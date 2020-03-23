//
// Created by yongge on 20-2-21.
//

#include <MessagePool.h>
#include "VideoFilter.h"


using namespace freee;


VideoFilter::VideoFilter(const std::string &mediaName, int mediaType)
        : MessageChain(mediaName, mediaType) {
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

//void VideoFilter::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
//    if (m_srcImageFormat != m_codecImageFormat
//        || m_srcWidth != m_codecWidth
//        || m_srcHeight != m_codecHeight){
//        if (p_bufferPool){
//            SmartPkt y420 = p_bufferPool->GetPkt(PktMsgProcessMedia);
//            if (y420.GetDataPtr()){
//                sr_buffer_frame_set_image_format(&y420.frame, y420.GetDataPtr(), m_codecWidth, m_codecHeight, m_codecImageFormat);
//                sr_buffer_frame_convert_to_yuv420p(&pkt.frame, &y420.frame, m_srcRotation);
//                y420.frame.timestamp = pkt.frame.timestamp;
//                MediaChainImpl::ProcessMedia(chain, y420);
//            }else {
//                LOGD("[WARNING] missed a video frame\n");
//            }
//        }
//    }else {
//        MediaChainImpl::ProcessMedia(chain, pkt);
//    }
//}

void VideoFilter::onMsgOpen(Message pkt) {
    m_config = static_cast<MessageChain *>(pkt.GetPtr())->GetConfig(this);
    if (m_status == Status_Closed){
        OpenModule();
        MessageChain::onMsgOpen(pkt);
        m_status = Status_Opened;
    }
}

void VideoFilter::onMsgClose(Message pkt) {
    if (m_status == Status_Opened){
        CloseModule();
        MessageChain::onMsgClose(pkt);
        m_status = Status_Closed;
    }
}

void VideoFilter::onMsgProcessData(Message pkt) {
    ProcessMediaByModule(pkt);
}

int VideoFilter::OpenModule() {
    LOGD("[MediaConfig] VideoFilter::ModuleOpen >>> %s\n", m_config.dump().c_str());
    m_srcWidth = m_config["srcWidth"];
    m_srcHeight = m_config["srcHeight"];
    m_srcRotation = m_config["srcRotation"];
    m_codecWidth = m_config["codecWidth"];
    m_codecHeight = m_config["codecHeight"];
    std::string srcFormat = m_config["srcImageFormat"];
    std::string codecFormat = m_config["codecImageFormat"];
    union {
        uint32_t format;
        unsigned char fourcc[4];
    }fourcctoint;
    memcpy(&fourcctoint.fourcc[0], srcFormat.c_str(), 4);
    m_srcImageFormat = fourcctoint.format;
    memcpy(&fourcctoint.fourcc[0], codecFormat.c_str(), 4);
    m_codecImageFormat = fourcctoint.format;
//    LOGD("VideoSource::UpdateMediaConfig src[%d] codec[%d]\n", m_srcImageFormat, libyuv::FOURCC_NV21);
    m_bufferSize = m_codecWidth * m_codecHeight / 2 * 3U;
    p_bufferPool = new MessagePool(m_bufferSize, 10, 64, 0, 0, "VideoFilter");
    return 0;
}

void VideoFilter::CloseModule() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

int VideoFilter::ProcessMediaByModule(Message pkt) {
    MessageChain::onMsgProcessData(pkt);
    return 0;
}
