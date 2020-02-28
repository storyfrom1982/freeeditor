//
// Created by yongge on 20-2-22.
//

#include <GLES3/gl3.h>
#include <BufferPool.h>
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
    isSurfaceCreated = false;
    isSurfaceDestroyed = true;
    renderer = nullptr;
    opengles = nullptr;
    mVideoWindow = nullptr;
    width = height = 0;
    StartProcessor(mediaName);
}

VideoRenderer::~VideoRenderer() {
    Close(this);
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
        mVideoWindow->SetCallback(nullptr);
        delete mVideoWindow;
        mVideoWindow = nullptr;
    }
}

void VideoRenderer::MessageOpen(SmartPkt pkt) {
    mConfig = static_cast<MediaChain *>(pkt.msg.ptr)->GetConfig(this);
    if (mStatus == Status_Closed){
        if (ModuleOpen(mConfig) != 0){
            return;
        }
        mStatus = Status_Opened;
        onOpened();
    }
}

void VideoRenderer::MessageClose(SmartPkt pkt) {
    if (mStatus == Status_Opened){
        ModuleClose();
        mStatus = Status_Closed;
        onClosed();
    }
}

void VideoRenderer::MessageProcessMedia(SmartPkt pkt) {
    ModuleProcessMedia(pkt);
}

void VideoRenderer::MessageControl(SmartPkt pkt) {
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

int VideoRenderer::ModuleOpen(json &cfg) {
    int width = cfg["codecWidth"];
    int height = cfg["codecHeight"];
    renderer = gl_renderer_create(width, height);
    if (!renderer){
        return -1;
    }
    opengles_open(&opengles);
    if (mVideoWindow){
        mVideoWindow->SetCallback(this);
    }
    return 0;
}

void VideoRenderer::ModuleClose() {
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

int VideoRenderer::ModuleProcessMedia(SmartPkt pkt) {
    AutoLock lock(mLock);
    if (!isSurfaceDestroyed){
        opengles_render(opengles, &pkt.frame);
        gl_renderer_swap_buffers(renderer);
    }
    return 0;
}

void VideoRenderer::SetVideoWindow(void *ptr) {
    ProcessMessage(SmartPkt(RecvMsg_SetVideoWindow, ptr));
}

void VideoRenderer::MessageSetVideoWindow(SmartPkt pkt) {
    mVideoWindow = new VideoWindow(static_cast<MessageContext *>(pkt.msg.ptr));
    if (mStatus == Status_Opened){
        mVideoWindow->SetCallback(this);
    }
}

void VideoRenderer::onSurfaceCreated(void *ptr) {
    AutoLock lock(mLock);
    ProcessMessage(SmartPkt(RecvMsg_SurfaceCreated, ptr));
}

void VideoRenderer::onSurfaceChanged(int width, int height) {
//    assert(width != 0 && height != 0);
    SmartPkt pkt(RecvMsg_SurfaceChanged);
    pkt.frame.width = width;
    pkt.frame.height = height;
    ProcessMessage(pkt);
}

void VideoRenderer::onSurfaceDestroyed() {
    AutoLock lock(mLock);
    isSurfaceDestroyed = true;
    ProcessMessage(SmartPkt(RecvMsg_SurfaceDestroyed));
}

void VideoRenderer::MessageWindowCreated(SmartPkt pkt) {
    isSurfaceCreated = true;
    isSurfaceDestroyed = false;
    if (renderer){
        gl_renderer_set_window(renderer, pkt.msg.ptr);
    }
}

void VideoRenderer::MessageWindowDestroyed(SmartPkt pkt) {
    if (isSurfaceCreated){
        isSurfaceCreated = false;
        if (renderer){
            gl_renderer_remove_window(renderer);
        }
    }
}

void VideoRenderer::MessageWindowChanged(SmartPkt pkt) {
    if (mVideoWindow){
        UpdateViewport(pkt.frame.width, pkt.frame.height);
    }
}

void VideoRenderer::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

