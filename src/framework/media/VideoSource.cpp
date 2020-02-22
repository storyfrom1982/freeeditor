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
    mPool = nullptr;
    mStatus = Status_Closed;
    if (context == nullptr){
        context = MediaContext::Instance().ConnectCamera();
    }
    ConnectContext(context);
}

VideoSource::~VideoSource() {
    LOGD("VideoSource::~VideoSource enter\n");
    SendMessage(MediaPacket(SendMsg_Close));
    DisconnectContext();
    if (mPool){
        delete mPool;
        mPool = nullptr;
    }
    LOGD("VideoSource::~VideoSource exit\n");
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    mMediaConfig = chain->GetMediaConfig(this);
    std::string cfg = mMediaConfig.dump();
    LOGD("VideoSource::Open: %s\n", cfg.c_str());
    MediaPacket pkt(SendMsg_Open);
    pkt.msg.json = strdup(cfg.c_str());
    SendMessage(pkt);
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MediaChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(MediaPacket(SendMsg_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MediaChain *chain) {
    LOGD("VideoSource::Start enter\n");
    SendMessage(MediaPacket(SendMsg_Start));
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop(MediaChain *chain) {
    LOGD("VideoSource::Stop enter\n");
    SendMessage(MediaPacket(SendMsg_Stop));
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::Control(MediaChain *chain, MediaPacket pkt) {

}

void VideoSource::ProcessMedia(MediaChain *chain, MediaPacket pkt) {
    sr_buffer_frame_fill_picture(&pkt.frame, (uint8_t*)pkt.msg.ptr, mSrcWidth, mSrcHeight, libyuv::FOURCC_NV21);
    MediaPacket y420;
    if (mPool){
        y420 = mPool->GetBuffer();
    }
    if (y420.buffer){
        sr_buffer_frame_fill_picture(&y420.frame, y420.buffer->data, mCodecWidth, mCodecHeight, libyuv::FOURCC_I420);
        sr_buffer_frame_to_yuv420p(&pkt.frame, &y420.frame, mSrcRotation);
        OutputMediaPacket(y420);
    }
}

void VideoSource::onRecvMessage(MediaPacket pkt) {
    switch (pkt.msg.key){
        case OnRecvMsg_ProcessPicture:
            ProcessMedia(this, pkt);
            break;
        case OnRecvMsg_Opened:
            mStatus = Status_Opened;
            LOGD("VideoSource Opened\n");
            UpdateMediaConfig(pkt);
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

void VideoSource::UpdateMediaConfig(MediaPacket pkt) {
    LOGD("VideoSource::UpdateMediaConfig >> %s\n", pkt.msg.json);
    mMediaConfig = json::parse(pkt.msg.json);
    mSrcWidth = mMediaConfig["srcWidth"];
    mSrcHeight = mMediaConfig["srcHeight"];
    mSrcRotation = mMediaConfig["srcRotation"];
    mCodecWidth = mMediaConfig["codecWidth"];
    mCodecHeight = mMediaConfig["codecHeight"];
    mBufferSize = mCodecWidth * mCodecHeight / 2 * 3;
    mPool = new MediaBufferPool(1, mBufferSize);
}
