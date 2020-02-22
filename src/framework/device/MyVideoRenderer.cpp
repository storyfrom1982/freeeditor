//
// Created by yongge on 20-2-4.
//
#include <BufferPool.h>
#include "MyVideoRenderer.h"
#include "MyVideoWindow.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <GLES2/gl2.h>

#ifdef __cplusplus
}
#endif


using namespace freee;


MyVideoRenderer::MyVideoRenderer() {
    std::string name("MyVideoRenderer");
    StartProcessor(name);
}

MyVideoRenderer::~MyVideoRenderer() {
    LOGD("MyVideoRenderer)\n");
    StopProcessor();
    if (renderer){
        gl_renderer_release(&renderer);
    }
    if (opengles){
        opengles_close(&opengles);
    }
}

void MyVideoRenderer::init(SmartPkt pkt) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void MyVideoRenderer::setSurfaceView(SmartPkt pkt) {
    MyVideoWindow *window = (MyVideoWindow*)pkt.msg.ptr;
    window->RegisterCallback(this);
}

void MyVideoRenderer::drawPicture(SmartPkt pkt) {
    opengles_render(opengles, &pkt.frame);
    gl_renderer_swap_buffers(renderer);
}

void MyVideoRenderer::surfaceCreated(SmartPkt pkt) {
    MyVideoWindow *window = (MyVideoWindow*)pkt.msg.ptr;
    gl_renderer_set_window(renderer, window->GetNativeWindow());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void MyVideoRenderer::surfaceChanged(SmartPkt msg) {

}

void MyVideoRenderer::surfaceDestroyed(SmartPkt msg) {
    AutoLock lock(m_lock);
    gl_renderer_remove_window(renderer);
    lock.signal();
}

void MyVideoRenderer::MessageProcessor(SmartPkt pkt) {
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

void MyVideoRenderer::OnPutMessage(SmartPkt pkt) {
    if (pkt.msg.key == OpenGLESRender_SurfaceDestroyed){
        AutoLock lock(m_lock);
        PutMessage(pkt);
        lock.wait();
    }else {
        PutMessage(pkt);
    }
}

