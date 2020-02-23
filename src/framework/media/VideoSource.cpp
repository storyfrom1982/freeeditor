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
    mBufferPool = nullptr;
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
    if (mBufferPool){
        delete mBufferPool;
        mBufferPool = nullptr;
    }
    LOGD("VideoSource::~VideoSource exit\n");
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    mMediaConfig = chain->GetMediaConfig(this);
    SmartMsg msg(SendMsg_Open, mMediaConfig.dump());
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
    if (mBufferPool){
        sr_buffer_frame_fill_picture(&pkt.frame, (uint8_t*)pkt.msg.GetPtr(), mSrcWidth, mSrcHeight, libyuv::FOURCC_NV21);
        SmartPkt y420 = mBufferPool->GetPkt();
        if (y420.buffer){
            sr_buffer_frame_fill_picture(&y420.frame, y420.buffer->data, mCodecWidth, mCodecHeight, libyuv::FOURCC_I420);
            sr_buffer_frame_to_yuv420p(&pkt.frame, &y420.frame, mSrcRotation);
            OutputMediaPacket(y420);
        }
    }
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
    LOGD("VideoSource::UpdateMediaConfig >> %s\n", msg.GetJson().c_str());
    mMediaConfig = json::parse(msg.GetJson());
    mSrcWidth = mMediaConfig["srcWidth"];
    mSrcHeight = mMediaConfig["srcHeight"];
    mSrcRotation = mMediaConfig["srcRotation"];
    mCodecWidth = mMediaConfig["codecWidth"];
    mCodecHeight = mMediaConfig["codecHeight"];
    mBufferSize = mCodecWidth * mCodecHeight / 2 * 3;
    mBufferPool = new BufferPool(10, mBufferSize);
}
