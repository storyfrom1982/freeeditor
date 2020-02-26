//
// Created by yongge on 20-2-5.
//

#include <BufferPool.h>
#include "VideoEncoder.h"
#include "X264VideoEncoder.h"


using namespace freee;


VideoEncoder *VideoEncoder::Create(std::string name) {
    return new X264VideoEncoder;
}

VideoEncoder::VideoEncoder(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    bufferPool = nullptr;
    StartProcessor(mediaName);
}

VideoEncoder::~VideoEncoder() {
    StopProcessor();
    FinalClear();
}

void VideoEncoder::FinalClear() {
    if (bufferPool){
        delete bufferPool;
        bufferPool = nullptr;
    }
}

void VideoEncoder::MessageOpen(SmartPkt pkt) {
    MediaChain *chain = static_cast<MediaChain *>(pkt.msg.ptr);
    mConfig = chain->GetConfig(this);
    ModuleOpen(mConfig);
    int w = mConfig["codecWidth"];
    int h = mConfig["codecHeight"];
    bufferPool = new BufferPool(10, w*h);
}

void VideoEncoder::MessageClose(SmartPkt pkt) {
    ModuleClose();
}

void VideoEncoder::MessageProcessMedia(SmartPkt pkt) {
    ModuleProcessMedia(pkt);
}

void VideoEncoder::MessageControl(SmartPkt pkt) {
    MediaChainImpl::MessageControl(pkt);
}
