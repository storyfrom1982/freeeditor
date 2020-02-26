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
    mStatus = Status_Closed;
    if (context == nullptr){
        context = MediaContext::Instance().ConnectCamera();
    }
    ConnectContext(context);
}

VideoSource::~VideoSource() {
    LOGD("VideoSource::~VideoSource enter\n");
    DisconnectContext();
    MediaContext::Instance().DisconnectCamera();
    LOGD("VideoSource::~VideoSource exit\n");
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    mConfig = chain->GetConfig(this);
    std::string str = mConfig.dump();
    SmartPkt pkt(SendMsg_Open, str.c_str(), str.length());
    LOGD("VideoSource::Open: %s\n", pkt.msg.json);
    SendMessage(pkt);
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
    sr_buffer_frame_set_color_space(
            &pkt.frame, (uint8_t *) pkt.msg.ptr,
            mSrcWidth, mSrcHeight, mSrcImageFormat);
    onProcessMedia(pkt);
}

void VideoSource::onRecvMessage(SmartPkt pkt) {
    switch (pkt.msg.key){
        case OnRecvMsg_ProcessPicture:
            ProcessMedia(this, pkt);
            break;
        case OnRecvMsg_Opened:
            mStatus = Status_Opened;
            LOGD("VideoSource Opened\n");
            UpdateMediaConfig(pkt);
            ReportEvent(SmartPkt(Status_Opened + mNumber));
            break;
        case OnRecvMsg_Closed:
            mStatus = Status_Closed;
            ReportEvent(SmartPkt(Status_Closed + mNumber));
            LOGD("VideoSource Closed\n");
            break;
        case OnRecvMsg_Started:
            mStatus = Status_Started;
            ReportEvent(SmartPkt(Status_Started + mNumber));
            LOGD("VideoSource Started\n");
            break;
        case OnRecvMsg_Stopped:
            ReportEvent(SmartPkt(Status_Stopped + mNumber));
            mStatus = Status_Stopped;
            LOGD("VideoSource Stopped\n");
            break;
        default:
            break;
    }
}

void VideoSource::UpdateMediaConfig(SmartPkt pkt) {
//    LOGD("VideoSource::UpdateMediaConfig >> %s\n", pkt.msg.json);
    mConfig = json::parse(pkt.msg.json);
    mSrcWidth = mConfig["srcWidth"];
    mSrcHeight = mConfig["srcHeight"];
    std::string srcFormat = mConfig["srcImageFormat"];
    union {
        int format;
        char fourcc[4];
    }fourcctoint;
    memcpy(&fourcctoint.fourcc[0], srcFormat.c_str(), 4);
    mSrcImageFormat = fourcctoint.format;
    onOpened();
}
