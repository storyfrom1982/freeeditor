//
// Created by yongge on 20-2-21.
//

#include "VideoFilter.h"


using namespace freee;


enum {
    RecvMsg_None = 0,
    RecvMsg_Open = 1,
    RecvMsg_Start = 2,
    RecvMsg_Stop = 3,
    RecvMsg_Close = 4,
    RecvMsg_ProcessMedia = 5,
};


VideoFilter::VideoFilter(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    mStatus = Status_Closed;
    StartProcessor(mediaName);
}

VideoFilter::~VideoFilter() {
    StopProcessor();
}

void VideoFilter::Open(MediaChain *chain) {
    mMediaConfig = chain->GetMediaConfig(this);
    ProcessMessage(MediaPacket(RecvMsg_Open));
}

void VideoFilter::Close(MediaChain *chain) {
    ProcessMessage(MediaPacket(RecvMsg_Close));
}

void VideoFilter::Start(MediaChain *chain) {
    mMediaConfig = chain->GetMediaConfig(this);
    ProcessMessage(MediaPacket(RecvMsg_Start));
}

void VideoFilter::Stop(MediaChain *chain) {
    ProcessMessage(MediaPacket(RecvMsg_Stop));
}

void VideoFilter::ProcessMedia(MediaChain *chain, MediaPacket pkt) {
    pkt.msg.ptr = chain;
    ProcessMessage(pkt);
}

void VideoFilter::MessageProcess(MediaPacket pkt) {
    switch (pkt.msg.key){
        case RecvMsg_Open:
            ProcessOpen();
            break;
        case RecvMsg_Close:
            ProcessClose();
            break;
        case RecvMsg_Start:
            ProcessStart();
            break;
        case RecvMsg_Stop:
            ProcessStop();
            break;
        case RecvMsg_ProcessMedia:
            ProcessPacket(pkt);
            break;
        default:
            break;
    }
}

void VideoFilter::ProcessOpen() {
    if (mStatus == Status_Closed){
        ModuleOpen(mMediaConfig);
        mStatus = Status_Opened;
    }
}

void VideoFilter::ProcessClose() {
    if (mStatus == Status_Started){
        ProcessStop();
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        ModuleClose();
        mStatus = Status_Closed;
    }
}

void VideoFilter::ProcessStart() {
    if (mStatus != Status_Opened){
        ProcessOpen();
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        mStatus = Status_Started;
    }
}

void VideoFilter::ProcessStop() {
    if (mStatus == Status_Started){
        mStatus = Status_Stopped;
    }
}

void VideoFilter::ProcessPacket(MediaPacket pkt) {
    if (mStatus != Status_Started){
        auto *chain = static_cast<MediaChain *>(pkt.msg.ptr);
        mMediaConfig = chain->GetMediaConfig(this);
        ProcessStart();
        if (mStatus != Status_Started){
            return;
        }
    }
    ModuleProcessPacket(pkt);
}

int VideoFilter::ModuleOpen(json &cfg) {
    int width = cfg["codecWidth"];
    int height = cfg["codecHeight"];
    pool = new MediaBufferPool(1,  width * height / 2 * 3);
    return 0;
}

void VideoFilter::ModuleClose() {
    if (pool){
        delete pool;
        pool = nullptr;
    }
}

int VideoFilter::ModuleProcessPacket(MediaPacket pkt) {
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
