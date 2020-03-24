//
// Created by yongge on 20-3-18.
//

#include "FFmpegAudioDecoder.h"


using namespace freee;

FFmpegAudioDecoder::FFmpegAudioDecoder()
{

}

FFmpegAudioDecoder::~FFmpegAudioDecoder()
{
    StopProcessor();
}

int FFmpegAudioDecoder::OpenDecoder()
{
    AVCodecID codecId = m_config["codecId"];
    AVCodec *codec = avcodec_find_decoder(codecId);
    if (codec == nullptr){
        LOGD("avcodec_find_decoder(%s) failed\n", avcodec_get_name(codecId));
        return -1;
    }

    m_pCodecContext = avcodec_alloc_context3(codec);
    m_pCodecContext->time_base.den = 1;
    m_pCodecContext->time_base.num = 1;

    if (m_extraConfig.size() > 0){
        m_pCodecContext->extradata_size = m_extraConfig.size();
        m_pCodecContext->extradata = (uint8_t*)av_malloc(m_pCodecContext->extradata_size);
        memcpy(m_pCodecContext->extradata, m_extraConfig.c_str(),
               m_pCodecContext->extradata_size);
    }


//    m_pCodecContext->opaque = this;
//    m_pCodecContext->get_buffer2 = get_buffer2;
    m_pCodecContext->thread_count = 1;
    m_pCodecContext->active_thread_type = FF_THREAD_SLICE;
    m_pCodecContext->block_align = m_config["codecBlockAlign"];
    m_pCodecContext->sample_rate = m_config["codecSampleRate"];
    m_pCodecContext->channels = m_config["codecChannelCount"];
    m_pCodecContext->bit_rate = m_config["codecBitRate"];

    if ( avcodec_open2( m_pCodecContext, codec, NULL)){
        LOGD("avcodec_open2(%s) failed\n", avcodec_get_name(codecId));
        avcodec_free_context(&m_pCodecContext);
        return -1;
    }

    return 0;
}

void FFmpegAudioDecoder::CloseDecoder()
{
    if (m_pCodecContext) {
        AVFrame              picture;
        int                  got_picture = 0;
        AVPacket             avpkt;
        ::av_init_packet( &avpkt );
        memset(&picture, 0, sizeof(picture));
        avpkt.size = 0;
        avpkt.data = NULL;
        do {
            avcodec_send_packet(m_pCodecContext, &avpkt);
            int ret = 0;
            while (ret >= 0) {
                ret = avcodec_receive_frame(m_pCodecContext, &picture);
                if (ret < 0) {
                    break;
                }else {
                    av_frame_unref(&picture);
                }
            }
            char buffer[1024];
            av_strerror(ret, buffer, sizeof(buffer) - 1);
            LOGD("[FFmpegAudioDecoder] avcodec_receive_frame() failed [%s]\n", buffer);
            if (ret == AVERROR_EOF)
            {
                break;
            }
        }while (1);
        avcodec_flush_buffers(m_pCodecContext);
        avcodec_close(m_pCodecContext);
        avcodec_free_context(&m_pCodecContext);
        m_pCodecContext = nullptr;
    }
    if (m_pBufferPool){
        delete m_pBufferPool;
        m_pBufferPool = nullptr;
    }
}

int FFmpegAudioDecoder::DecodeAudio(Message msg)
{
    AVPacket             avpkt;
    ::av_init_packet( &avpkt );
    avpkt.stream_index = msg.GetFramePtr()->index;
    avpkt.data = msg.GetDataPtr();
    avpkt.size = msg.GetMsgLength();
    avpkt.dts =  msg.GetFramePtr()->timestamp;
    avpkt.pts  = msg.GetFramePtr()->timestamp;
    avpkt.flags = 0;
    avpkt.buf = nullptr;

    AVFrame              picture;
    int                  got_picture = 0;
    memset(&picture, 0, sizeof(picture));


    int64_t startTime = sr_time_begin();

    int ret;
    ret = avcodec_send_packet(m_pCodecContext, &avpkt);
    if (ret < 0) {
        if (ret == AVERROR(EAGAIN))
        {
            ret = 0;
        }
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(m_pCodecContext, &picture);
        if (ret < 0) {
            break;
        }else {
//            if (frameMap.find(picture.opaque) != frameMap.end()){
//                Message message = frameMap[picture.opaque];
//                frameMap.erase(picture.opaque);
//                MessageChain::onMsgProcessData(message);
//            }
//            LOGD("FFmpegVideoDecoder::DecodeAudio data size %d\n", picture.linesize[0]);
            Message message = NewDataMessage(MsgKey_ProcessData, picture.data[0], picture.linesize[0]);
            MessageChain::onMsgProcessData(message);
            av_frame_unref(&picture);
        }
    }
//    LOGD("FFmpegAudioDecoder::DecodeAudio data size %lu\n", msg.GetMsgLength());
    return 0;
}

int FFmpegAudioDecoder::get_buffer2(struct AVCodecContext *s, AVFrame *frame, int flags)
{
    return 0;
}
