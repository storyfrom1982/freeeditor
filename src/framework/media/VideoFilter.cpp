//
// Created by yongge on 20-2-21.
//

#include "../message/MessagePool.h"
#include "VideoFilter.h"


using namespace freee;


VideoFilter::VideoFilter(const std::string name) : MessageChain(name) {
    m_type = MediaType_Video;
    m_status = Status_Closed;
    p_bufferPool = nullptr;
    StartProcessor();
}

VideoFilter::~VideoFilter() {
    StopProcessor();
    FinalClear();
}

void VideoFilter::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

void VideoFilter::onMsgOpen(Message pkt) {
    m_config = static_cast<MessageChain *>(pkt.obj())->GetConfig(this);
    if (m_status == Status_Closed){
        OpenMedia();
        MessageChain::onMsgOpen(pkt);
        m_status = Status_Opened;
    }
}

void VideoFilter::onMsgClose(Message pkt) {
    if (m_status == Status_Opened){
        CloseMedia();
        MessageChain::onMsgClose(pkt);
        m_status = Status_Closed;
    }
}

void VideoFilter::onMsgProcessData(Message pkt) {
    ProcessMedia(pkt);
}

int VideoFilter::OpenMedia() {
    LOGD("[MediaConfig] VideoFilter::ModuleOpen >>> %s\n", m_config.dump(4).c_str());
    m_srcWidth = m_config[CFG_SRC_WIDTH];
    m_srcHeight = m_config[CFG_SRC_HEIGHT];
    m_srcRotation = m_config[CFG_SRC_ROTATION];
    m_codecWidth = m_config[CFG_CODEC_WIDTH];
    m_codecHeight = m_config[CFG_CODEC_HEIGHT];
    std::string srcFormat = m_config[CFG_SRC_IMAGE_FORMAT];
    std::string codecFormat = m_config[CFG_CODEC_IMAGE_FORMAT];
    m_srcImageFormat = libyuv_convert_fourcc(srcFormat.c_str());
    m_codecImageFormat = libyuv_convert_fourcc(codecFormat.c_str());
    m_bufferSize = m_codecWidth * m_codecHeight / 2 * 3U;
    p_bufferPool = new MessagePool(GetName() + "FramePool", m_bufferSize, 10, 64, 0, 0);
    return 0;
}

void VideoFilter::CloseMedia() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

int VideoFilter::ProcessMedia(Message msg) {

    if (m_srcImageFormat != m_codecImageFormat
        || m_srcWidth != m_codecWidth
        || m_srcHeight != m_codecHeight){
        if (p_bufferPool){
            Message y420 = p_bufferPool->NewMessage(MsgKey_ProcessData);
            libyuv_set_format(y420.msgFrame(), y420.data(), m_codecWidth,
                              m_codecHeight, m_codecImageFormat);
            libyuv_convert_to_yuv420p(msg.msgFrame(), y420.msgFrame(), m_srcRotation);
            y420.msgFrame()->timestamp = msg.msgFrame()->timestamp;
            MessageChain::onMsgProcessData(y420);
        }
    }else {
        MessageChain::onMsgProcessData(msg);
    }

    return 0;
}
