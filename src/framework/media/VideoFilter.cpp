//
// Created by yongge on 20-2-21.
//

#include <BufferPool.h>
#include "VideoFilter.h"


using namespace freee;


VideoFilter::VideoFilter(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    m_status = Status_Closed;
    p_bufferPool = nullptr;
    StartProcessor(mediaName);
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

void VideoFilter::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    if (m_srcImageFormat != m_codecImageFormat
        || m_srcWidth != m_codecWidth
        || m_srcHeight != m_codecHeight){
        if (p_bufferPool){
            SmartPkt y420 = p_bufferPool->GetPkt(PktMsgProcessMedia);
            if (y420.GetDataPtr()){
                sr_buffer_frame_set_image_format(&y420.frame, y420.GetDataPtr(), m_codecWidth, m_codecHeight, m_codecImageFormat);
                sr_buffer_frame_convert_to_yuv420p(&pkt.frame, &y420.frame, m_srcRotation);
                y420.frame.timestamp = pkt.frame.timestamp;
                MediaChainImpl::ProcessMedia(chain, y420);
            }else {
                LOGD("[WARNING] missed a video frame\n");
            }
        }
    }else {
        MediaChainImpl::ProcessMedia(chain, pkt);
    }
}

void VideoFilter::MessageOpen(SmartPkt pkt) {
    m_config = static_cast<MediaChain *>(pkt.GetPtr())->GetConfig(this);
    if (m_status == Status_Closed){
        ModuleOpen(m_config);
        onOpened();
        m_status = Status_Opened;
    }
}

void VideoFilter::MessageClose(SmartPkt pkt) {
    if (m_status == Status_Opened){
        ModuleClose();
        onClosed();
        m_status = Status_Closed;
    }
}

void VideoFilter::MessageProcessMedia(SmartPkt pkt) {
    ModuleProcessMedia(pkt);
}

int VideoFilter::ModuleOpen(json &cfg) {
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
    p_bufferPool = new BufferPool(2, m_bufferSize, 10);
    p_bufferPool->SetName(m_name);
    return 0;
}

void VideoFilter::ModuleClose() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

int VideoFilter::ModuleProcessMedia(SmartPkt pkt) {
    onProcessMedia(pkt);
    return 0;
}
