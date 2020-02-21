//
// Created by yongge on 20-2-4.
//
#include <MediaBufferPool.h>
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

void VideoRenderer::init(MediaPacket pkt) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void VideoRenderer::setSurfaceView(MediaPacket pkt) {
    VideoWindow *window = (VideoWindow*)pkt.msg.ptr;
    window->RegisterCallback(this);
}

void VideoRenderer::drawPicture(MediaPacket pkt) {
    opengles_render(opengles, &pkt.frame);
    gl_renderer_swap_buffers(renderer);
}

void VideoRenderer::surfaceCreated(MediaPacket pkt) {
    VideoWindow *window = (VideoWindow*)pkt.msg.ptr;
    gl_renderer_set_window(renderer, window->GetNativeWindow());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void VideoRenderer::surfaceChanged(MediaPacket msg) {

}

void VideoRenderer::surfaceDestroyed(MediaPacket msg) {
    AutoLock lock(m_lock);
    gl_renderer_remove_window(renderer);
    lock.signal();
}

void VideoRenderer::MessageProcessor(MediaPacket pkt) {
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

void VideoRenderer::OnPutMessage(MediaPacket pkt) {
    if (pkt.msg.key == OpenGLESRender_SurfaceDestroyed){
        AutoLock lock(m_lock);
        PutMessage(pkt);
        lock.wait();
    }else {
        PutMessage(pkt);
    }
}

