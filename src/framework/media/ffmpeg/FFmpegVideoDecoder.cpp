//
// Created by yongge on 20-3-18.
//


#include "FFmpegVideoDecoder.h"


using namespace freee;

FFmpegVideoDecoder::FFmpegVideoDecoder()
{

}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
    StopProcessor();
}

int FFmpegVideoDecoder::OpenDecoder()
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

    m_pCodecContext->thread_type = FF_THREAD_SLICE;
    m_pCodecContext->thread_count = 1;

    m_pCodecContext->opaque = this;
    m_pCodecContext->get_buffer2 = get_buffer2;
    m_pCodecContext->width = m_config["codecWidth"];
    m_pCodecContext->height = m_config["codecHeight"];
    m_pCodecContext->bit_rate = m_config["codecBitRate"];
    m_pCodecContext->profile = m_config["codecProfile"];
    m_pCodecContext->level = m_config["codecLevel"];
    m_pCodecContext->pix_fmt = m_config["codecFormat"];

    if ( avcodec_open2( m_pCodecContext, codec, NULL)){
        LOGD("avcodec_open2(%s) failed\n", avcodec_get_name(codecId));
        avcodec_free_context(&m_pCodecContext);
        return -1;
    }

    return 0;
}

void FFmpegVideoDecoder::CloseDecoder()
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
            LOGD("[FFmpegVideoDecoder] avcodec_receive_frame() failed [%s]\n", buffer);
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

int FFmpegVideoDecoder::DecodeVideo(Message msg)
{
    AVPacket             avpkt;
    ::av_init_packet( &avpkt );
    avpkt.stream_index = msg.GetFramePtr()->index;
    avpkt.data = msg.GetBufferPtr();
    avpkt.size = msg.length();
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
            if (frameMap.find(picture.opaque) != frameMap.end()){
                Message message = frameMap[picture.opaque];
                frameMap.erase(picture.opaque);
//                LOGD("FFmpegVideoDecoder::DecodeVideo data size %d\n", message.frame.width);
                MessageChain::onMsgProcessData(message);
            }
            av_frame_unref(&picture);
        }
    }

//    LOGD("FFmpegVideoDecoder::DecodeVideo decode time %lld\n", sr_time_passed(startTime));

    return 0;

}

int FFmpegVideoDecoder::decode(AVCodecContext *avctx, AVFrame *picture,
                           int *got_picture_ptr,
                           const AVPacket *avpkt) {
    *got_picture_ptr = false;
    int ret = avcodec_send_packet(avctx, avpkt);
    if (ret < 0) {
        if (AVERROR(EAGAIN) != ret) {
            return -1;
        }
        ret = 0;
    }
    ret = avcodec_receive_frame(avctx, picture);
    if (ret < 0) {
        if (AVERROR(EAGAIN) != ret) {
            return -1;
        }
        ret = 0;
    }
    else
        *got_picture_ptr = true;
    return ret;
}

int FFmpegVideoDecoder::get_buffer2(struct AVCodecContext *s, AVFrame *frame, int flags)
{
    FFmpegVideoDecoder *pThis = (FFmpegVideoDecoder*)s->opaque;
    pThis->GetVideoBuffer(frame);
    return 0;
}

void FFmpegVideoDecoder::GetVideoBuffer(AVFrame *frame)
{
//    int64_t startTime = sr_time_begin();
    CreateBufferPool(frame);

    Message msg = m_pBufferPool->NewMessage(MsgKey_ProcessData);
    int pos = 0;
    for (int i = 0; i < m_planeCount; i++) {
        msg.GetFramePtr()->channel[i].data = msg.GetBufferPtr() + pos;
        msg.GetFramePtr()->channel[i].size = m_planeSize[i];
        pos += m_planeSize[i];
    }
    struct op {
        static void cb(void *opaque, uint8_t *data) {

        }
    };

    for (int i = 0; i < this->m_planeCount; i++) {
        frame->buf[i] = av_buffer_create(msg.GetFramePtr()->channel[i].data, msg.GetFramePtr()->channel[i].size,
                                         &op::cb,
                                         this, 0);
        frame->data[i] = msg.GetFramePtr()->channel[i].data;
        msg.GetFramePtr()->channel[i].stride = frame->linesize[i];
    }

//    msg.frame.width = frame->linesize[0];
    msg.GetFramePtr()->width = frame->width;
    msg.GetFramePtr()->height = frame->height;
    frame->opaque = msg.GetBufferPtr();

    frameMap[frame->opaque] = msg;
//    LOGD("FFmpegVideoDecoder::get_buffer2 time %lld\n", sr_time_passed(startTime));
}

void FFmpegVideoDecoder::CreateBufferPool(AVFrame *frame)
{
    int linesize[AV_NUM_DATA_POINTERS];
    int w = frame->width;
    int h = frame->height;
    int unaligned;
    int stride_align[AV_NUM_DATA_POINTERS];
    avcodec_align_dimensions2(m_pCodecContext, &w, &h, stride_align);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get((AVPixelFormat)frame->format);

    do {
        w += w & ~(w - 1);
        av_image_fill_linesizes(linesize, (AVPixelFormat)frame->format, w);
        unaligned = 0;
        for (int i = 0; i < 4; i++) {
            unaligned |= linesize[i] % stride_align[i];
            frame->linesize[i] = linesize[i];
        }
    } while (unaligned);
    int i;
    for (  i = 0; i < 4 && frame->linesize[i]; i++) {
        int h = FFALIGN(frame->height, 32);
        if (i == 1 || i == 2)
            h = FF_CEIL_RSHIFT(h, desc->log2_chroma_h);
        m_planeSize[i] = linesize[i] * (h + 2);
        m_bufferSize += m_planeSize[i];
    }
    int flags = desc ? desc->flags : 0;
    if (i == 1 && (flags & AV_PIX_FMT_FLAG_PAL)) {
        m_planeSize[1] = 4096;
        i = 2;
    }

    m_planeCount = i;

    if (!m_pBufferPool){
        m_pBufferPool = new MessagePool(GetName() + "FramePool", m_bufferSize, 1, 64, 0, 16);
    }
}
