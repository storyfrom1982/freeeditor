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

void VideoRenderer::init(SrMessage msg) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void VideoRenderer::setSurfaceView(SrMessage msg) {
    VideoWindow *window = (VideoWindow*)msg.frame.data;
    window->RegisterCallback(this);
}

void VideoRenderer::drawPicture(SrMessage msg) {
    opengles_render(opengles, &msg.frame);
    gl_renderer_swap_buffers(renderer);
}

void VideoRenderer::surfaceCreated(SrMessage msg) {
    VideoWindow *window = (VideoWindow*)msg.frame.data;
    gl_renderer_set_window(renderer, (gl_window_t*) window->GetNativeWindow());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void VideoRenderer::surfaceChanged(SrMessage msg) {

}

void VideoRenderer::surfaceDestroyed(SrMessage msg) {
    AutoLock lock(m_lock);
    gl_renderer_remove_window(renderer);
    lock.signal();
}

void VideoRenderer::MessageProcessor(SrMessage msg) {
    switch (msg.frame.key){
        case OpenGLESRender_Init:
            init(msg);
            break;
        case OpenGLESRender_SetSurfaceView:
            setSurfaceView(msg);
            break;
        case OpenGLESRender_DrawPicture:
            drawPicture(msg);
            break;
        case OpenGLESRender_SurfaceCreated:
            surfaceCreated(msg);
            break;
        case OpenGLESRender_SurfaceDestroyed:
            surfaceDestroyed(msg);
            break;
        default:
            break;
    }
}

void VideoRenderer::OnPutMessage(SrMessage msg) {
    if (msg.frame.key == OpenGLESRender_SurfaceDestroyed){
        AutoLock lock(m_lock);
        PutMessage(msg);
        lock.wait();
    }else {
        PutMessage(msg);
    }
}

