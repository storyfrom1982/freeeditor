//
// Created by yongge on 20-2-21.
//

#include "VideoFilter.h"


using namespace freee;


VideoFilter::VideoFilter(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    mStatus = Status_Closed;
    StartProcessor(mediaName);
}

VideoFilter::~VideoFilter() {
    StopProcessor();
}

void VideoFilter::MessageOpen(MediaPacket pkt) {
    mMediaConfig = static_cast<MediaChain *>(pkt.msg.ptr)->GetMediaConfig(this);
    if (mStatus == Status_Closed){
        ModuleImplOpen(mMediaConfig);
        mStatus = Status_Opened;
    }
}

void VideoFilter::MessageClose(MediaPacket pkt) {
    if (mStatus == Status_Started){
        MessageStop(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        ModuleImplClose();
        mStatus = Status_Closed;
    }
}

void VideoFilter::MessageStart(MediaPacket pkt) {
    if (mStatus != Status_Opened){
        MessageOpen(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        mStatus = Status_Started;
    }
}

void VideoFilter::MessageStop(MediaPacket pkt) {
    if (mStatus == Status_Started){
        mStatus = Status_Stopped;
    }
}

void VideoFilter::MessagePacket(MediaPacket pkt) {
    if (mStatus != Status_Started){
        MessageStart(pkt);
        if (mStatus != Status_Started){
            return;
        }
    }
    ModuleImplProcessMedia(pkt);
}

int VideoFilter::ModuleImplOpen(json &cfg) {
    int width = cfg["codecWidth"];
    int height = cfg["codecHeight"];
    pool = new MediaBufferPool(1,  width * height / 2 * 3);
    return 0;
}

void VideoFilter::ModuleImplClose() {
    if (pool){
        delete pool;
        pool = nullptr;
    }
}

int VideoFilter::ModuleImplProcessMedia(MediaPacket pkt) {
    int srcWidth = mMediaConfig["srcWidth"];
    int srcHeight = mMediaConfig["srcHeight"];
    int codecWidth = mMediaConfig["codecWidth"];
    int codecHeight = mMediaConfig["codecHeight"];
    sr_buffer_frame_fill_picture(&pkt.frame, pkt.buffer->data, srcWidth, srcHeight, libyuv::FOURCC_NV21);
    MediaPacket y420 = pool->GetBuffer();
    sr_buffer_frame_fill_picture(&y420.frame, y420.buffer->data, codecWidth, codecHeight, libyuv::FOURCC_I420);
    sr_buffer_frame_to_yuv420p(&pkt.frame, &y420.frame, mMediaConfig["srcRotation"]);
    OutputMediaPacket(pkt);
    return 0;
}
