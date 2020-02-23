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
    FinalClear();
}

void VideoRenderer::FinalClear() {
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

void VideoRenderer::MessageOpen(SmartPkt pkt) {
    mMediaConfig = static_cast<MediaChain *>(pkt.msg.GetPtr())->GetMediaConfig(this);
    if (mStatus == Status_Closed){
        ModuleImplOpen(mMediaConfig);
        mStatus = Status_Opened;
    }
}

void VideoRenderer::MessageClose(SmartPkt pkt) {
    if (mStatus == Status_Started){
        MessageStop(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        ModuleImplClose();
        mStatus = Status_Closed;
    }
}

void VideoRenderer::MessageStart(SmartPkt pkt) {
    if (mStatus != Status_Opened){
        MessageOpen(pkt);
    }
    if (mStatus == Status_Opened || mStatus == Status_Stopped){
        mStatus = Status_Started;
    }
}

void VideoRenderer::MessageStop(SmartPkt pkt) {
    if (mStatus == Status_Started){
        mStatus = Status_Stopped;
    }
}

void VideoRenderer::MessagePacket(SmartPkt pkt) {
    if (mStatus != Status_Started){
        MessageStart(pkt);
        if (mStatus != Status_Started){
            return;
        }
    }
    ModuleImplProcessMedia(pkt);
}

void VideoRenderer::MessageControl(SmartPkt pkt) {
    switch (pkt.msg.GetKey()){
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
    int width = cfg["codecWidth"];
    int height = cfg["codecHeight"];
    renderer = gl_renderer_create(width, height);
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

int VideoRenderer::ModuleImplProcessMedia(SmartPkt pkt) {
    AutoLock lock(mLock);
    if (!isWindowPausing){
        opengles_render(opengles, &pkt.frame);
        gl_renderer_swap_buffers(renderer);
    }
    return 0;
}

void VideoRenderer::SetVideoWindow(void *ptr) {
    LOGD("VideoRenderer::SetVideoWindow enter\n");
    ProcessMessage(SmartPkt(SmartMsg(RecvMsg_SetVideoWindow, ptr)));
    LOGD("VideoRenderer::SetVideoWindow exit\n");
}

void VideoRenderer::MessageSetVideoWindow(SmartPkt pkt) {
    LOGD("VideoRenderer::MessageSetVideoWindow enter\n");
    mVideoWindow = new VideoWindow(static_cast<MessageContext *>(pkt.msg.GetPtr()));
    mVideoWindow->SetCallback(this);
    LOGD("VideoRenderer::MessageSetVideoWindow exit\n");
}

void VideoRenderer::onSurfaceCreated(void *ptr) {
    LOGD("VideoRenderer::onSurfaceCreated enter\n");
    AutoLock lock(mLock);
    isWindowPausing = false;
    ProcessMessage(SmartPkt(SmartMsg(RecvMsg_SurfaceCreated, ptr)));
    LOGD("VideoRenderer::onSurfaceCreated exit\n");
}

void VideoRenderer::onSurfaceChanged() {
    ProcessMessage(SmartPkt(SmartMsg(RecvMsg_SurfaceChanged)));
}

void VideoRenderer::onSurfaceDestroyed() {
    AutoLock lock(mLock);
    isWindowPausing = true;
    ProcessMessage(SmartPkt(SmartMsg(RecvMsg_SurfaceDestroyed)));
}

void VideoRenderer::MessageWindowCreated(SmartPkt pkt) {
    isWindowReady = true;
    if (mStatus == Status_Opened){
        gl_renderer_set_window(renderer, pkt.msg.GetPtr());
    }
}

void VideoRenderer::MessageWindowDestroyed(SmartPkt pkt) {
    if (isWindowReady){
        isWindowReady = false;
        if (mStatus == Status_Opened){
            gl_renderer_remove_window(renderer);
        }
    }
}

void VideoRenderer::MessageWindowChanged(SmartPkt pkt) {
    if (mVideoWindow){
        UpdateViewport(mVideoWindow->width(), mVideoWindow->height());
    }
}

void VideoRenderer::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

