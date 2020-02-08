//
// Created by yongge on 19-6-16.
//

#include "VideoSource.h"

#ifdef __ANDROID__
#include <Camera.h>
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <libyuv.h>

//#ifdef __cplusplus
//}
//#endif


using namespace freee;

freee::VideoSource *freee::VideoSource::openVideoSource(MessageContext *ctx) {

    VideoSource *camera = NULL;

#ifdef __ANDROID__
    camera = new Camera(ctx);
#endif

    return camera;
}

VideoSource::VideoSource() {
}

VideoSource::~VideoSource() {

}

void VideoSource::setEncoder(MessageContext *ctx) {
    outputCtx = ctx;
}

void VideoSource::processData(void *data, int size) {

}

void VideoSource::processData(sr_message_t msg) {
    msg.key = 8;
    msg.size = MessageType_Pointer;
    outputCtx->OnPutDataBuffer(msg);
}

