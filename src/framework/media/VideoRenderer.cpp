//
// Created by yongge on 20-2-22.
//

#include <GLES3/gl3.h>
#include "VideoRenderer.h"


using namespace freee;

enum {
    RecvMsg_SetVideoWindow = RecvMsg_Control + 1,
    RecvMsg_SurfaceCreated,
    RecvMsg_SurfaceChanged,
    RecvMsg_SurfaceDestroyed,
};

VideoRenderer::VideoRenderer(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    mStatus = Status_Closed;
    isWindowPausing = false;
    isWindowReady = false;
    renderer = nullptr;
    opengles = nullptr;
    mVideoWindow = nullptr;
    StartProcessor(mediaName);
}

VideoRenderer::~VideoRenderer() {
    Stop(this);
    StopProcessor();
}

void VideoRenderer::MessageOpen(MediaPacket pkt) {
    mMediaConfig = static_cast<MediaChain *>(pkt.msg.ptr)->GetMediaConfig(this);
    if (mStatus == Status_Closed){
        ModuleImplOpen(mMediaConfig);
        mStatus = Status_Opened;
    }
}

void VideoRenderer::MessageClose(MediaPacket pkt) {
    if (mStatus == Status_Started){
        MessageStop(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        ModuleImplClose();
        mStatus = Status_Closed;
    }
}

void VideoRenderer::MessageStart(MediaPacket pkt) {
    if (mStatus != Status_Opened){
        MessageOpen(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        mStatus = Status_Started;
    }
}

void VideoRenderer::MessageStop(MediaPacket pkt) {
    if (mStatus == Status_Started){
        mStatus = Status_Stopped;
    }
}

void VideoRenderer::MessagePacket(MediaPacket pkt) {
    if (mStatus != Status_Started){
        MessageStart(pkt);
        if (mStatus != Status_Started){
            return;
        }
    }
    ModuleImplProcessMedia(pkt);
}

void VideoRenderer::MessageControl(MediaPacket pkt) {
    switch (pkt.msg.key){
        case RecvMsg_SetVideoWindow:
            MessageSetVideoWindow(pkt);
            break;
        case RecvMsg_SurfaceCreated:
            MessageWindowCreated(pkt);
            break;
        case RecvMsg_SurfaceChanged:
            MessageWindowChanged(pkt);
            break;
        case RecvMsg_SurfaceDestroyed:
            MessageWindowDestroyed(pkt);
            break;
        default:
            break;
    }
}

int VideoRenderer::ModuleImplOpen(json &cfg) {
    renderer = gl_renderer_create(360, 640);
    opengles_open(&opengles);
    if (isWindowReady && mVideoWindow){
        gl_renderer_set_window(renderer, mVideoWindow->window());
        UpdateViewport(mVideoWindow->width(), mVideoWindow->height());
    }
    return 0;
}

void VideoRenderer::ModuleImplClose() {
    if (renderer){
        gl_renderer_release(&renderer);
    }
    if (opengles){
        opengles_close(&opengles);
    }
    if (mVideoWindow){
        delete mVideoWindow;
        mVideoWindow = nullptr;
    }
}

int VideoRenderer::ModuleImplProcessMedia(MediaPacket pkt) {
    AutoLock lock(mLock);
    if (!isWindowPausing){
        opengles_render(opengles, &pkt.frame);
        gl_renderer_swap_buffers(renderer);
    }
    return 0;
}

void VideoRenderer::SetVideoWindow(MediaPacket pkt) {
    LOGD("VideoRenderer::SetVideoWindow enter\n");
    pkt.msg.key = RecvMsg_SetVideoWindow;
    ProcessMessage(pkt);
    LOGD("VideoRenderer::SetVideoWindow exit\n");
}

void VideoRenderer::MessageSetVideoWindow(MediaPacket pkt) {
    LOGD("VideoRenderer::MessageSetVideoWindow enter\n");
    mVideoWindow = new VideoWindow(static_cast<MessageContext *>(pkt.msg.ptr));
    mVideoWindow->SetCallback(this);
    LOGD("VideoRenderer::MessageSetVideoWindow exit\n");
}

void VideoRenderer::onSurfaceCreated(MediaPacket msg) {
    LOGD("VideoRenderer::onSurfaceCreated enter\n");
    AutoLock lock(mLock);
    isWindowPausing = false;
    msg.msg.key = RecvMsg_SurfaceCreated;
    ProcessMessage(msg);
    LOGD("VideoRenderer::onSurfaceCreated exit\n");
}

void VideoRenderer::onSurfaceChanged(MediaPacket msg) {
    msg.msg.key = RecvMsg_SurfaceChanged;
    ProcessMessage(msg);
}

void VideoRenderer::onSurfaceDestroyed(MediaPacket msg) {
    AutoLock lock(mLock);
    isWindowPausing = true;
    msg.msg.key = RecvMsg_SurfaceDestroyed;
    ProcessMessage(msg);
}

void VideoRenderer::MessageWindowCreated(MediaPacket pkt) {
    isWindowReady = true;
    if (mStatus == Status_Opened){
        gl_renderer_set_window(renderer, pkt.msg.ptr);
    }
}

void VideoRenderer::MessageWindowDestroyed(MediaPacket pkt) {
    if (isWindowReady){
        isWindowReady = false;
        if (mStatus == Status_Opened){
            gl_renderer_remove_window(renderer);
        }
    }
}

void VideoRenderer::MessageWindowChanged(MediaPacket pkt) {
    if (mVideoWindow){
        UpdateViewport(mVideoWindow->width(), mVideoWindow->height());
    }
}

void VideoRenderer::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

