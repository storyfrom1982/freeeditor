//
// Created by yongge on 20-3-15.
//

#include "FFmpegMediaSource.h"


using namespace freee;

FFmpegMediaSource::FFmpegMediaSource()
{

}

FFmpegMediaSource::~FFmpegMediaSource()
{
    StopProcessor();
    CloseSource();
}

int FFmpegMediaSource::OpenSource(Message msg)
{
    int result;
    AVInputFormat *iformat = NULL;
    m_pContext = avformat_alloc_context();
    m_interruptCallback.opaque = this;

    struct Callback
    {
        static int break_callback(void* context){
            FFmpegMediaSource *pThis = (FFmpegMediaSource*)context;
            return pThis->m_isAbort;
        }
    };

    m_interruptCallback.callback = &Callback::break_callback;
    m_pContext->interrupt_callback = m_interruptCallback;

    if ((result = avformat_open_input(&m_pContext, "/storage/emulated/0/test.mp4", iformat, NULL)) < 0) {
        LOGD("[FFmpegMediaSource] avformat_open_input() failed\n");
        avformat_free_context(m_pContext);
        return result;
    }

    /* fill the streams in the format context */
    if ((result = avformat_find_stream_info(m_pContext, NULL)) < 0) {
        LOGD("[FFmpegMediaSource] avformat_find_stream_info() failed\n");
        avformat_close_input(&m_pContext);
        return result;
    }

    long long maxDuration = m_pContext->duration;

//    m_streamCount = m_pContext->nb_streams;

    /* bind a decoder to each input stream */
    for (int i = 0; i < m_pContext->nb_streams; i++) {

        AVStream *stream = m_pContext->streams[i];
        AVCodec *codec;

        long long streamDuration = stream->time_base.den == 0 ? -1 :
                                   (long long)stream->duration * stream->time_base.num * 1000000 / stream->time_base.den;
        if (maxDuration < streamDuration){
            maxDuration = streamDuration;
        }

        AVCodecParameters *codecpar  = stream->codecpar;

        if (!(codec = avcodec_find_decoder(codecpar->codec_id))) {
            LOGE("[FFmpegMediaSource] Unsupported codec (id=%d) for input stream %d\n",
                   codecpar->codec_id, stream->index);
        }

        json cfg;
        cfg["streamId"] = stream->index;
        cfg["codecId"] = codecpar->codec_id;
        cfg["codecType"] = codecpar->codec_type;
        cfg["codecFormat"] = codecpar->format;
        cfg["codecProfile"] = codecpar->profile;
        cfg["codecLevel"] = codecpar->level;
        cfg["codecTag"] = std::string((char*)&codecpar->codec_tag, 4);


        if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            cfg["codecWidth"] = codecpar->width;
            cfg["codecHeight"] = codecpar->height;
            cfg["codecPixelWidth"] = codecpar->sample_aspect_ratio.num;
            cfg["codecPixelHeight"] = codecpar->sample_aspect_ratio.den;
            cfg["codecBitRate"] = codecpar->bit_rate;
            if (stream->avg_frame_rate.den != 0) {
                cfg["codecFrameRate"] = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
            }
            if (codecpar->extradata_size > 0){
                //todo paser avc1
                m_extraConfigMap[stream->index] = std::string((char*)codecpar->extradata, codecpar->extradata_size);
            }
            m_config[std::to_string(stream->index)] = cfg;
        }
        else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            cfg["codecChannelCount"] = codecpar->channels;
            cfg["codecSampleRate"] = codecpar->sample_rate;
            cfg["codecBytePerSample"] = av_get_bytes_per_sample((AVSampleFormat)codecpar->format);
            cfg["codecBitRate"] = codecpar->bit_rate;
            cfg["codecSamplePerFrame"] = codecpar->frame_size;
            cfg["codecBlockAlign"] = codecpar->block_align;
            if (codecpar->extradata_size > 0){
                m_extraConfigMap[stream->index] = std::string((char*)codecpar->extradata, codecpar->extradata_size);
            }
            m_config[std::to_string(stream->index)] = cfg;
        }
        else if (codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE){

        }

    }

    LOGD("MediaSource config: %s\n", m_config.dump(4).c_str());

    for (int i = 0; i < m_pContext->nb_streams; i++){
        AVCodecParameters *codecpar  = m_pContext->streams[i]->codecpar;
        if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            m_streamCount ++;
            Message streamConfig = NewJsonMessage(MsgKey_ProcessEvent, m_config[std::to_string(
                    m_pContext->streams[i]->index)].dump());
            SendEvent(streamConfig);
        }else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            m_streamCount ++;
            Message streamConfig = NewJsonMessage(MsgKey_ProcessEvent, m_config[std::to_string(
                    m_pContext->streams[i]->index)].dump());
            SendEvent(streamConfig);
        }
    }

    return 0;
}

void FFmpegMediaSource::CloseSource()
{
    avformat_close_input(&m_pContext);
}

int FFmpegMediaSource::ReadSource()
{
    AVPacket pkt = {0};
    int result = av_read_frame(m_pContext, &pkt);
    if (result < 0  ){
        if (result == AVERROR(EAGAIN) && m_isRunning){
            onReadSource(Message(1001));
            return 0;
        }
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer) - 1);
        LOGD("[FFmpegMediaSource] av_read_frame() failed [%s]\n", buffer);
        return -1;
    }
//    LOGD("[FFmpegMediaSource] av_read_frame() stream id %d\n", pkt.stream_index);

    Message msg = NewDataMessage(MsgKey_ProcessData, pkt.data, pkt.size);
    msg.frame.index = pkt.stream_index;
    msg.frame.timestamp = 0;
    ProcessMessage(msg);

    av_packet_unref(&pkt);

    return 0;
}
