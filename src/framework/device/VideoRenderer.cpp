//
// Created by yongge on 20-2-4.
//
#include <SrBufferPool.h>
#include "VideoRenderer.h"
#include "VideoWindow.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <GLES2/gl2.h>

#ifdef __cplusplus
}
#endif


using namespace freee;


VideoRenderer::VideoRenderer() {
    std::string name("VideoRenderer");
    StartProcessor(name);
}

VideoRenderer::~VideoRenderer() {
    LOGD("VideoRenderer)\n");
    StopProcessor();
    if (renderer){
        gl_renderer_release(&renderer);
    }
    if (opengles){
        opengles_close(&opengles);
    }
}

void VideoRenderer::init(SrPkt pkt) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void VideoRenderer::setSurfaceView(SrPkt pkt) {
    VideoWindow *window = (VideoWindow*)pkt.msg.ptr;
    window->RegisterCallback(this);
}

void VideoRenderer::drawPicture(SrPkt pkt) {
    opengles_render(opengles, &pkt.frame);
    gl_renderer_swap_buffers(renderer);
}

void VideoRenderer::surfaceCreated(SrPkt pkt) {
    VideoWindow *window = (VideoWindow*)pkt.msg.ptr;
    gl_renderer_set_window(renderer, (gl_window_t*) window->GetNativeWindow());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void VideoRenderer::surfaceChanged(SrPkt msg) {

}

void VideoRenderer::surfaceDestroyed(SrPkt msg) {
    AutoLock lock(m_lock);
    gl_renderer_remove_window(renderer);
    lock.signal();
}

void VideoRenderer::MessageProcessor(SrPkt pkt) {
    switch (pkt.msg.key){
        case OpenGLESRender_Init:
            init(pkt);
            break;
        case OpenGLESRender_SetSurfaceView:
            setSurfaceView(pkt);
            break;
        case OpenGLESRender_DrawPicture:
            drawPicture(pkt);
            break;
        case OpenGLESRender_SurfaceCreated:
            surfaceCreated(pkt);
            break;
        case OpenGLESRender_SurfaceDestroyed:
            surfaceDestroyed(pkt);
            break;
        default:
            break;
    }
}

void VideoRenderer::OnPutMessage(SrPkt pkt) {
    if (pkt.msg.key == OpenGLESRender_SurfaceDestroyed){
        AutoLock lock(m_lock);
        PutMessage(pkt);
        lock.wait();
    }else {
        PutMessage(pkt);
    }
}

