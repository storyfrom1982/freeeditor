//
// Created by yongge on 20-2-5.
//

#include "../message/MessagePool.h"
#include "VideoEncoder.h"
#include "encoder/X264VideoEncoder.h"


using namespace freee;


VideoEncoder *VideoEncoder::Create(std::string name) {
    return new X264VideoEncoder;
}

VideoEncoder::VideoEncoder(const std::string name) : MessageChain(name) {
    m_type = MediaType_Video;
    p_bufferPool = nullptr;
    StartProcessor();
}

VideoEncoder::~VideoEncoder() {
    LOGE("VideoEncoder::~VideoEncoder ===================== : enter\n");
    StopProcessor();
    LOGE("VideoEncoder::~VideoEncoder ===================== : 1\n");
    FinalClear();
    LOGE("VideoEncoder::~VideoEncoder ===================== : exit\n");
}

void VideoEncoder::FinalClear() {
    LOGE("VideoEncoder::FinalClear ===================== : enter\n");
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
//    CloseEncoder();
    LOGE("VideoEncoder::FinalClear ===================== : exit\n");
}

void VideoEncoder::onMsgOpen(Message pkt) {
    if (m_status == Status_Closed){
        MessageChain *chain = static_cast<MessageChain *>(pkt.GetObjectPtr());
        m_config = chain->GetConfig(this);
        OpenEncoder();
        m_frameId = 0;
        m_startFrameId = -1;
        m_frameRate = m_config["codecFPS"];
        uint32_t w = m_config["codecWidth"];
        uint32_t h = m_config["codecHeight"];
        p_bufferPool = new MessagePool("VideoEncoderFramePool", w*h, 1, 64, 16, 0);
        pkt.GetFramePtr()->type = MediaType_Video;
        MessageChain::onMsgOpen(pkt);
        m_status = Status_Opened;
    }
}

void VideoEncoder::onMsgClose(Message pkt) {
    LOGE("VideoEncoder::onMsgClose ===================== : enter\n");
//    MessageChain::onMsgClose(pkt);
    CloseEncoder();
    LOGE("VideoEncoder::onMsgClose ===================== : exit\n");
}

void VideoEncoder::onMsgProcessData(Message pkt) {

    if ( m_chainStatus == Status_Opened) {
//        LOGD("m_framerate ================  delay[%f]\n", m_frameRate);
        long long timeStamp = pkt.GetFramePtr()->timestamp;
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
                EncoderEncode(pkt);
            }
        }
        if (error < -0.555) {// drop
//            LOGD("drop a video frame[%lf]\n", error);
            return ;
        }

        m_frameId++;
        EncoderEncode(pkt);
    }
}

void VideoEncoder::onMsgControl(Message pkt) {
    MessageChain::onMsgControl(pkt);
}

void VideoEncoder::onMsgProcessEvent(Message pkt) {
    switch (pkt.event()){
        case MsgKey_Open:
            m_chainStatus = Status_Opened;
            break;
        case MsgKey_Close:
            m_chainStatus = Status_Closed;
            break;
        default:
            break;
    }
}
