//
// Created by yongge on 20-2-5.
//

#include <BufferPool.h>
#include "VideoEncoder.h"
#include "X264VideoEncoder.h"


using namespace freee;


VideoEncoder *VideoEncoder::Create(std::string name) {
    return new X264VideoEncoder;
}

VideoEncoder::VideoEncoder(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    p_bufferPool = nullptr;
    StartProcessor(mediaName);
}

VideoEncoder::~VideoEncoder() {
    StopProcessor();
    FinalClear();
}

void VideoEncoder::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
    CloseModule();
}

void VideoEncoder::onMsgOpen(Message pkt) {
    MessageChain *chain = static_cast<MessageChain *>(pkt.GetPtr());
    m_config = chain->GetConfig(this);
    OpenModule();
    m_frameId = 0;
    m_startFrameId = -1;
    m_frameRate = m_config["codecFPS"];
    uint32_t w = m_config["codecWidth"];
    uint32_t h = m_config["codecHeight"];
    p_bufferPool = new BufferPool(4, w*h, 256, 16);
    p_bufferPool->SetName(m_name);
    pkt.frame.type = MediaType_Video;
    MessageChain::onMsgOpen(pkt);
}

void VideoEncoder::onMsgClose(Message pkt) {
    CloseModule();
    MessageChain::onMsgClose(pkt);
}

void VideoEncoder::onMsgProcessData(Message pkt) {

    if ( m_outputChainStatus == Status_Opened) {
//        LOGD("m_framerate ================  delay[%f]\n", m_frameRate);
        long long timeStamp = pkt.frame.timestamp;
        double frameIdScope = (double) timeStamp * (m_frameRate / 1000000.0f);
        if (m_startFrameId == -1) {
            m_startFrameId = frameIdScope;
            m_frameId = frameIdScope;
        }
        double error = frameIdScope - m_frameId;

        if (error > 10 || error < -10) {
            m_startFrameId = -1;
            return ;
        }
        if (error > 0.555) {// fill
            for (int i = 0; i < error; i++) {
//                LOGD("fill a video frame[%lf]\n", error);
                m_frameId++;
                ProcessMediaByModule(pkt);
            }
        }
        if (error < -0.555) {// drop
//            LOGD("drop a video frame[%lf]\n", error);
            return ;
        }

        m_frameId++;
        ProcessMediaByModule(pkt);
    }
}

void VideoEncoder::onMsgControl(Message pkt) {
    MessageChain::onMsgControl(pkt);
}

void VideoEncoder::onMsgProcessEvent(Message pkt) {
    switch (pkt.GetEvent()){
        case MsgKey_Open:
            m_outputChainStatus = Status_Opened;
            break;
        case MsgKey_Close:
            m_outputChainStatus = Status_Closed;
            break;
        default:
            break;
    }
}
