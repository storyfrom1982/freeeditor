//
// Created by yongge on 20-2-4.
//
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
    SetContextName("VideoRenderer");
    StartMessageProcessor();
}

VideoRenderer::~VideoRenderer() {
    LOGD("VideoRenderer)\n");
    StopMessageProcessor();
    if (renderer){
        gl_renderer_release(&renderer);
    }
    if (opengles){
        opengles_close(&opengles);
    }
}

void VideoRenderer::OnPutMessage(sr_message_t msg) {

    if (msg.key == OpenGLESRender_SurfaceDestroyed){
        AutoLock lock(m_lock);
        ProcessMessage(msg);
        lock.wait();
    }else {
        ProcessMessage(msg);
    }
}

sr_message_t VideoRenderer::OnGetMessage(sr_message_t msg) {
    return sr_message_t();
}

void VideoRenderer::MessageProcessor(sr_message_t msg) {
    switch (msg.key){
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

//void VideoRenderer::messageProcessorLoop(sr_message_processor_t *processor, sr_message_t msg) {
//
//    switch (msg.key){
//        case OpenGLESRender_Init:
//            init(msg);
//            break;
//        case OpenGLESRender_SetSurfaceView:
//            setSurfaceView(msg);
//            break;
//        case OpenGLESRender_DrawPicture:
//            drawPicture(msg);
//            break;
//        default:
//            break;
//    }
//}

//void VideoRenderer::messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg) {
//    ((VideoRenderer*)processor->handler)->messageProcessorLoop(processor, msg);
//}

void VideoRenderer::init(sr_message_t msg) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void VideoRenderer::setSurfaceView(sr_message_t msg) {
    VideoWindow *window = (VideoWindow*)msg.ptr;
    window->RegisterCallback(this);
}

void VideoRenderer::drawPicture(sr_message_t msg) {
    sr_buffer_t *buffer = reinterpret_cast<sr_buffer_t *>(msg.str);
    opengles_render(opengles, (const VideoPacket*)buffer->ptr);
    gl_renderer_swap_buffers(renderer);
    sr_buffer_pool_free(buffer);
}

void VideoRenderer::surfaceCreated(sr_message_t msg) {
    VideoWindow *window = (VideoWindow*)msg.ptr;
    gl_renderer_set_window(renderer, (gl_window_t*) window->GetNativeWindow());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void VideoRenderer::surfaceChanged(sr_message_t msg) {

}

void VideoRenderer::surfaceDestroyed(sr_message_t msg) {
    LOGD("VideoRenderer::surfaceDestroyed enter\n");
    AutoLock lock(m_lock);
    gl_renderer_remove_window(renderer);
    lock.signal();
    LOGD("VideoRenderer::surfaceDestroyed exit\n");
}

