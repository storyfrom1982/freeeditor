//
// Created by yongge on 20-2-20.
//

#include <android/MediaContext.h>
#include "VideoSource.h"

using namespace std;
using namespace freee;


enum {
    SendMsg_Open = 1,
    SendMsg_Start,
    SendMsg_Stop,
    SendMsg_Close,
};

enum {
    OnRecvMsg_Opened = 1,
    OnRecvMsg_Started,
    OnRecvMsg_Stopped,
    OnRecvMsg_Closed,
    OnRecvMsg_ProcessPicture,
};


VideoSource::VideoSource(MessageContext *context)
    : MessageChain("VideoSource", MediaType_Video) {
    m_status = Status_Closed;
    if (context == nullptr){
//        context = MediaContext::Instance().ConnectCamera();
        context = MediaContext::Instance()->ConnectCamera();
    }
    ConnectContext(context);
}

VideoSource::~VideoSource() {
    DisconnectContext();
//    MediaContext::Instance().DisconnectCamera();
    MediaContext::Instance()->DisconnectCamera();
    FinalClear();
}

void VideoSource::Open(MessageChain *chain) {
    LOGD("VideoSource::Open enter\n");
    m_config = chain->GetConfig(this);
    SendMessage(NewJsonMessage(SendMsg_Open, m_config.dump()));
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MessageChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(NewFrameMessage(SendMsg_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MessageChain *chain) {
    LOGD("VideoSource::Start enter\n");
    SendMessage(NewFrameMessage(SendMsg_Start));
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop(MessageChain *chain) {
    LOGD("VideoSource::Stop enter\n");
    SendMessage(NewFrameMessage(SendMsg_Stop));
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::ProcessData(MessageChain *chain, Message pkt) {
    sr_buffer_frame_set_image_format(
            pkt.GetFramePtr(), pkt.GetFramePtr()->data,
            m_srcWidth, m_srcHeight, m_srcImageFormat);
    if (m_srcImageFormat != m_codecImageFormat
        || m_srcWidth != m_codecWidth
        || m_srcHeight != m_codecHeight){
        if (p_bufferPool){
            Message y420 = p_bufferPool->NewMessage(MsgKey_ProcessData);
            if (y420.GetDataPtr()){
                sr_buffer_frame_set_image_format(y420.GetFramePtr(), y420.GetDataPtr(), m_codecWidth, m_codecHeight, m_codecImageFormat);
                sr_buffer_frame_convert_to_yuv420p(pkt.GetFramePtr(), y420.GetFramePtr(), m_srcRotation);
                y420.GetFramePtr()->timestamp = pkt.GetFramePtr()->timestamp;
                MessageChain::onMsgProcessData(y420);
            }else {
                LOGD("[WARNING] missed a video frame\n");
            }
        }
    }else {
        MessageChain::onMsgProcessData(pkt);
    }
}

void VideoSource::onRecvMessage(Message pkt) {
    switch (pkt.GetKey()){
        case OnRecvMsg_ProcessPicture:
            ProcessData(this, pkt);
            break;
        case OnRecvMsg_Opened:
            m_status = Status_Opened;
            LOGD("VideoSource Opened\n");
            pkt.SetKey(MsgKey_Open);
            UpdateMediaConfig(pkt);
//            ReportEvent(SmartPkt(Status_Opened + m_number));
            break;
        case OnRecvMsg_Closed:
            m_status = Status_Closed;
            pkt.SetKey(MsgKey_Close);
            MessageChain::onMsgClose(pkt);
            FinalClear();
//            ReportEvent(SmartPkt(Status_Closed + m_number));
            LOGD("VideoSource Closed\n");
            break;
        case OnRecvMsg_Started:
            m_status = Status_Started;
//            ReportEvent(SmartPkt(Status_Started + m_number));
            LOGD("VideoSource Started\n");
            break;
        case OnRecvMsg_Stopped:
//            ReportEvent(SmartPkt(Status_Stopped + m_number));
            m_status = Status_Stopped;
            LOGD("VideoSource Stopped\n");
            break;
        default:
            break;
    }
}

void VideoSource::UpdateMediaConfig(Message pkt) {
//    LOGD("VideoSource::UpdateMediaConfig >> %s\n", pkt.msg.json);
    m_config = json::parse(pkt.GetString());
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
    p_bufferPool = new MessagePool(m_bufferSize, 10, 64 ,0, 0, "VideoSource");

    MessageChain::onMsgOpen(pkt);
}

void VideoSource::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}
