//
// Created by yongge on 20-2-21.
//

#include <BufferPool.h>
#include "VideoFilter.h"


using namespace freee;


VideoFilter::VideoFilter(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    mStatus = Status_Closed;
    mBufferPool = nullptr;
    StartProcessor(mediaName);
}

VideoFilter::~VideoFilter() {
    StopProcessor();
    FinalClear();
}

void VideoFilter::FinalClear() {
    if (mBufferPool){
        delete mBufferPool;
        mBufferPool = nullptr;
    }
}

void VideoFilter::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    if (mSrcImageFormat != mCodecImageFormat || mSrcWidth != mCodecWidth || mSrcHeight != mCodecHeight){
        if (mBufferPool){
//            sr_buffer_frame_set_color_space(&pkt.frame,
//                    (uint8_t *) pkt.msg.GetPtr(), mSrcWidth,mSrcHeight, mSrcImageFormat);
            SmartPkt y420 = mBufferPool->GetPkt();
            if (y420.buffer){
                sr_buffer_frame_set_color_space(&y420.frame,
                        y420.buffer->data, mCodecWidth, mCodecHeight, mCodecImageFormat);
                sr_buffer_frame_convert_to_yuv420p(&pkt.frame, &y420.frame, mSrcRotation);
                MediaChainImpl::ProcessMedia(chain, y420);
            }
        }
    }else {
        MediaChainImpl::ProcessMedia(chain, pkt);
    }
}

void VideoFilter::MessageOpen(SmartPkt pkt) {
    mConfig = static_cast<MediaChain *>(pkt.msg.ptr)->GetConfig(this);
    if (mStatus == Status_Closed){
        ModuleOpen(mConfig);
        onOpened();
        mStatus = Status_Opened;
    }
}

void VideoFilter::MessageClose(SmartPkt pkt) {
    if (mStatus == Status_Opened){
        ModuleClose();
        onClosed();
        mStatus = Status_Closed;
    }
}

void VideoFilter::MessageProcessMedia(SmartPkt pkt) {
    ModuleProcessMedia(pkt);
}

int VideoFilter::ModuleOpen(json &cfg) {
    LOGD("VideoFilter::UpdateConfig >> %s\n", mConfig.dump().c_str());
    mSrcWidth = mConfig["srcWidth"];
    mSrcHeight = mConfig["srcHeight"];
    mSrcRotation = mConfig["srcRotation"];
    mCodecWidth = mConfig["codecWidth"];
    mCodecHeight = mConfig["codecHeight"];
    std::string srcFormat = mConfig["srcImageFormat"];
    std::string codecFormat = mConfig["codecImageFormat"];
    union {
        int format;
        char fourcc[4];
    }fourcctoint;
    memcpy(&fourcctoint.fourcc[0], srcFormat.c_str(), 4);
    mSrcImageFormat = fourcctoint.format;
    memcpy(&fourcctoint.fourcc[0], codecFormat.c_str(), 4);
    mCodecImageFormat = fourcctoint.format;
//    LOGD("VideoSource::UpdateMediaConfig src[%d] codec[%d]\n", mSrcImageFormat, mCodecImageFormat);
    mBufferSize = mCodecWidth * mCodecHeight / 2 * 3;
    mBufferPool = new BufferPool(10, mBufferSize);
    return 0;
}

void VideoFilter::ModuleClose() {
    if (mBufferPool){
        delete mBufferPool;
        mBufferPool = nullptr;
    }
}

int VideoFilter::ModuleProcessMedia(SmartPkt pkt) {
    onProcessMedia(pkt);
    return 0;
}
