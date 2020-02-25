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
    SendMessage(SmartMsg(SendMsg_Close));
    DisconnectContext();
    MediaContext::Instance().DisconnectCamera();
    LOGD("VideoSource::~VideoSource exit\n");
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    mConfig = chain->GetConfig(this);
    SmartMsg msg(SendMsg_Open, mConfig.dump());
    LOGD("VideoSource::Open: %s\n", msg.GetJson().c_str());
    SendMessage(msg);
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MediaChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(SmartMsg(SendMsg_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MediaChain *chain) {
    LOGD("VideoSource::Start enter\n");
    SendMessage(SmartMsg(SendMsg_Start));
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop(MediaChain *chain) {
    LOGD("VideoSource::Stop enter\n");
    SendMessage(SmartMsg(SendMsg_Stop));
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    sr_buffer_frame_set_color_space(
            &pkt.frame, (uint8_t *) pkt.msg.GetPtr(),
            mSrcWidth, mSrcHeight, mSrcImageFormat);
    onProcessMedia(pkt);
}

void VideoSource::onRecvMessage(SmartMsg msg) {
    switch (msg.GetKey()){
        case OnRecvMsg_ProcessPicture:
            ProcessMedia(this, SmartPkt(msg));
            break;
        case OnRecvMsg_Opened:
            mStatus = Status_Opened;
            LOGD("VideoSource Opened\n");
            UpdateMediaConfig(msg);
            break;
        case OnRecvMsg_Closed:
            mStatus = Status_Closed;
            LOGD("VideoSource Closed\n");
            break;
        case OnRecvMsg_Started:
            mStatus = Status_Started;
            LOGD("VideoSource Started\n");
            break;
        case OnRecvMsg_Stopped:
            mStatus = Status_Stopped;
            LOGD("VideoSource Stopped\n");
            break;
        default:
            break;
    }
}

void VideoSource::UpdateMediaConfig(SmartMsg msg) {
//    LOGD("VideoSource::UpdateMediaConfig >> %s\n", msg.GetJson().c_str());
    mConfig = json::parse(msg.GetJson());
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
