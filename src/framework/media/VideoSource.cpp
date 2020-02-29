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
    : MediaChainImpl(MediaType_Video, MediaNumber_VideoSource, "VideoSource") {
    m_status = Status_Closed;
    if (context == nullptr){
//        context = MediaContext::Instance().ConnectCamera();
        context = MediaContext::Instance()->ConnectCamera();
    }
    ConnectContext(context);
    SetContextName("VideoSource");
}

VideoSource::~VideoSource() {
    DisconnectContext();
//    MediaContext::Instance().DisconnectCamera();
    MediaContext::Instance()->DisconnectCamera();
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    m_config = chain->GetConfig(this);
    SendMessage(NewJsonPkt(SendMsg_Open, m_config.dump()));
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MediaChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(SmartPkt(SendMsg_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MediaChain *chain) {
    LOGD("VideoSource::Start enter\n");
    SendMessage(SmartPkt(SendMsg_Start));
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop(MediaChain *chain) {
    LOGD("VideoSource::Stop enter\n");
    SendMessage(SmartPkt(SendMsg_Stop));
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    sr_buffer_frame_set_image_format(
            &pkt.frame, pkt.frame.data,
            m_srcWidth, m_srcHeight, m_srcImageFormat);
    onProcessMedia(pkt);
}

void VideoSource::onRecvMessage(SmartPkt pkt) {
    switch (pkt.GetKey()){
        case OnRecvMsg_ProcessPicture:
            ProcessMedia(this, pkt);
            break;
        case OnRecvMsg_Opened:
            m_status = Status_Opened;
            LOGD("VideoSource Opened\n");
            UpdateMediaConfig(pkt);
            ReportEvent(SmartPkt(Status_Opened + m_number));
            break;
        case OnRecvMsg_Closed:
            m_status = Status_Closed;
            ReportEvent(SmartPkt(Status_Closed + m_number));
            LOGD("VideoSource Closed\n");
            break;
        case OnRecvMsg_Started:
            m_status = Status_Started;
            ReportEvent(SmartPkt(Status_Started + m_number));
            LOGD("VideoSource Started\n");
            break;
        case OnRecvMsg_Stopped:
            ReportEvent(SmartPkt(Status_Stopped + m_number));
            m_status = Status_Stopped;
            LOGD("VideoSource Stopped\n");
            break;
        default:
            break;
    }
}

void VideoSource::UpdateMediaConfig(SmartPkt pkt) {
//    LOGD("VideoSource::UpdateMediaConfig >> %s\n", pkt.msg.json);
    m_config = json::parse(pkt.GetString());
    m_srcWidth = m_config["srcWidth"];
    m_srcHeight = m_config["srcHeight"];
    std::string srcFormat = m_config["srcImageFormat"];
    union {
        uint32_t format;
        char fourcc[4];
    }fourcctoint;
    memcpy(&fourcctoint.fourcc[0], srcFormat.c_str(), 4);
    m_srcImageFormat = fourcctoint.format;
    onOpened();
}

void VideoSource::FinalClear() {

}
