//
// Created by yongge on 20-2-22.
//

#include <GLES3/gl3.h>
#include "../message/MessagePool.h"
#include "VideoRenderer.h"


using namespace freee;

enum {
    RecvMsg_SetVideoWindow = MsgKey_ProcessControl + 1,
    RecvMsg_SurfaceCreated,
    RecvMsg_SurfaceChanged,
    RecvMsg_SurfaceDestroyed,
};

VideoRenderer::VideoRenderer(const std::string name) : MessageChain(name) {
    m_type = MediaType_Video;
    m_status = Status_Closed;
    isSurfaceCreated = false;
    isSurfaceDestroyed = true;
    renderer = nullptr;
    opengles = nullptr;
    mVideoWindow = nullptr;
    width = height = 0;
    StartProcessor();
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

void VideoRenderer::onMsgOpen(Message pkt) {
    m_config = static_cast<MessageChain *>(pkt.obj())->GetConfig(this);
    if (m_status == Status_Closed){
        if (OpenMedia() != 0){
            return;
        }
        m_status = Status_Opened;
        MessageChain::onMsgOpen(pkt);
    }
}

void VideoRenderer::onMsgClose(Message pkt) {
    if (m_status == Status_Opened){
        CloseMedia();
        m_status = Status_Closed;
        MessageChain::onMsgClose(pkt);
    }
}

void VideoRenderer::onMsgProcessData(Message pkt) {
    ProcessMedia(pkt);
}

void VideoRenderer::onMsgControl(Message pkt) {
    switch (pkt.key()){
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

int VideoRenderer::OpenMedia() {
    int width = m_config["codecWidth"];
    int height = m_config["codecHeight"];
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

void VideoRenderer::CloseMedia() {
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

int VideoRenderer::ProcessMedia(Message pkt) {
    AutoLock lock(mLock);
    if (!isSurfaceDestroyed){
//        int64_t startTime = sr_time_begin();
        opengles_render(opengles, pkt.msgFrame());
        gl_renderer_swap_buffers(renderer);
//        LOGD("FFmpegVideoDecoder::ProcessMediaByModule time %lld\n", sr_time_passed(startTime));
    }
    MessageChain::onMsgProcessData(pkt);
    return 0;
}

void VideoRenderer::SetVideoWindow(void *ptr) {
    ProcessMessage(NewMessage(RecvMsg_SetVideoWindow, ptr));
}

void VideoRenderer::MessageSetVideoWindow(Message pkt) {
    mVideoWindow = new VideoWindow(static_cast<MessageContext *>(pkt.obj()));
    if (m_status == Status_Opened){
        mVideoWindow->SetCallback(this);
    }
}

void VideoRenderer::onSurfaceCreated(void *ptr) {
    AutoLock lock(mLock);
    ProcessMessage(NewMessage(RecvMsg_SurfaceCreated, ptr));
}

void VideoRenderer::onSurfaceChanged(int width, int height) {
//    assert(width != 0 && height != 0);
    Message pkt = NewMessage(RecvMsg_SurfaceChanged);
    pkt.msgFrame()->width = width;
    pkt.msgFrame()->height = height;
    ProcessMessage(pkt);
}

void VideoRenderer::onSurfaceDestroyed() {
    AutoLock lock(mLock);
    isSurfaceDestroyed = true;
    ProcessMessage(NewMessage(RecvMsg_SurfaceDestroyed));
}

void VideoRenderer::MessageWindowCreated(Message pkt) {
    isSurfaceCreated = true;
    isSurfaceDestroyed = false;
    if (renderer){
        gl_renderer_set_window(renderer, pkt.obj());
    }
}

void VideoRenderer::MessageWindowDestroyed(Message pkt) {
    if (isSurfaceCreated){
        isSurfaceCreated = false;
        if (renderer){
            gl_renderer_remove_window(renderer);
        }
    }
}

void VideoRenderer::MessageWindowChanged(Message pkt) {
    if (mVideoWindow){
        UpdateViewport(pkt.msgFrame()->width, pkt.msgFrame()->height);
    }
}

void VideoRenderer::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

