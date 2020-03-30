//
// Created by yongge on 20-3-15.
//

#include "FFmpegMediaSource.h"


using namespace freee;

FFmpegMediaSource::FFmpegMediaSource() : MediaSource("FFmpegMediaSource")
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
        cfg[CFG_CODEC_ID] = codecpar->codec_id;
        cfg[CFG_CODEC_NAME] = std::string((char*)&codecpar->codec_tag, 4);
        cfg[CFG_CODEC_STREAM_ID] = stream->index;
        cfg[CFG_CODEC_TYPE] = codecpar->codec_type;
        cfg[CFG_CODEC_IMAGE_FORMAT] = codecpar->format;
        cfg[CFG_CODEC_PROFILE] = codecpar->profile;
        cfg[CFG_CODEC_LEVEL] = codecpar->level;

        if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            cfg[CFG_TYPE] = CFG_VIDEO;
            cfg[CFG_CODEC_WIDTH] = codecpar->width;
            cfg[CFG_CODEC_HEIGHT] = codecpar->height;
            cfg[CFG_CODEC_IMAGE_FORMAT_ID] = codecpar->format;
            cfg[CFG_CODEC_PIXEL_WIDTH] = codecpar->sample_aspect_ratio.num;
            cfg[CFG_CODEC_PIXEL_HEIGHT] = codecpar->sample_aspect_ratio.den;
            cfg[CFG_CODEC_BITRATE] = codecpar->bit_rate;
            if (stream->avg_frame_rate.den != 0) {
                cfg[CFG_CODEC_FRAME_RATE] = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
            }
            if (codecpar->extradata_size > 0){
                //todo paser avc1
                m_extraConfigMap[stream->index] = std::string((char*)codecpar->extradata, codecpar->extradata_size);
            }
            m_config[std::to_string(stream->index)] = cfg;
        }
        else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            cfg[CFG_TYPE] = CFG_AUDIO;
            cfg[CFG_CODEC_BITRATE] = codecpar->bit_rate;
            cfg[CFG_CODEC_CHANNEL_COUNT] = codecpar->channels;
            cfg[CFG_CODEC_SAMPLE_FORMAT_ID] = codecpar->format;
            cfg[CFG_CODEC_SAMPLE_RATE] = codecpar->sample_rate;
            cfg[CFG_CODEC_BYTES_PER_SAMPLE] = av_get_bytes_per_sample((AVSampleFormat)codecpar->format);
            cfg[CFG_CODEC_SAMPLES_PER_FRAME] = codecpar->frame_size;
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
            Message streamConfig = NewMessage(MsgKey_ProcessEvent, m_config[std::to_string(
                    m_pContext->streams[i]->index)].dump());
            SendEvent(streamConfig);
        }else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            m_streamCount ++;
            Message streamConfig = NewMessage(MsgKey_ProcessEvent, m_config[std::to_string(
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
    if (!m_pContext){
        return 0;
    }
    AVPacket pkt = {0};
    int result = av_read_frame(m_pContext, &pkt);
    if (result < 0  ){
        if (result == AVERROR(EAGAIN) && m_isRunning){
            onReadSource(NewMessage(1001));
            return 0;
        }
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer) - 1);
        LOGD("[FFmpegMediaSource] av_read_frame() failed [%s]\n", buffer);
        return -1;
    }
//    LOGD("[FFmpegMediaSource] av_read_frame() stream id %d\n", pkt.stream_index);

    Message msg = NewMessage(MsgKey_ProcessData, pkt.data, pkt.size);
    msg.GetFramePtr()->stream_id = pkt.stream_index;
    msg.GetFramePtr()->timestamp = 0;
    ProcessMessage(msg);

    av_packet_unref(&pkt);

    return 0;
}
