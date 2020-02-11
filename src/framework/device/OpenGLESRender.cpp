//
// Created by yongge on 20-2-4.
//
#include "OpenGLESRender.h"
#include "NativeWindow.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <GLES2/gl2.h>

#ifdef __cplusplus
}
#endif


using namespace freee;


OpenGLESRender::OpenGLESRender() {
    SetContextName("OpenGLESRender");
    StartMessageProcessor();
}

OpenGLESRender::~OpenGLESRender() {
    LOGD("~OpenGLESRender()\n");
    StopMessageProcessor();
    if (renderer){
        gl_renderer_release(&renderer);
    }
    if (opengles){
        opengles_close(&opengles);
    }
}

//void OpenGLESRender::OnPutMessage(sr_message_t msg) {
//    sr_message_queue_put(m_queue, msg);
//}

sr_message_t OpenGLESRender::OnGetMessage(sr_message_t msg) {
    return sr_message_t();
}

void OpenGLESRender::MessageProcessor(sr_message_t msg) {
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

//void OpenGLESRender::messageProcessorLoop(sr_message_processor_t *processor, sr_message_t msg) {
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

//void OpenGLESRender::messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg) {
//    ((OpenGLESRender*)processor->handler)->messageProcessorLoop(processor, msg);
//}

void OpenGLESRender::init(sr_message_t msg) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void OpenGLESRender::setSurfaceView(sr_message_t msg) {
    NativeWindow *window = (NativeWindow*)msg.ptr;
    window->SetStatusCallback(this);
}

void OpenGLESRender::drawPicture(sr_message_t msg) {
//    LOGD("drawPicture data size: %d", msg.key);
    opengles_render(opengles, (const VideoPacket*)msg.ptr);
    gl_renderer_swap_buffers(renderer);
//    videoPacket_Free(reinterpret_cast<VideoPacket **>(&msg.ptr));
}

void OpenGLESRender::surfaceCreated(sr_message_t msg) {
    NativeWindow *window = (NativeWindow*)msg.ptr;
    gl_renderer_set_window(renderer, (gl_window_t*) window->GetWindowHandler());
    int w, h;
    window->GetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void OpenGLESRender::surfaceChanged(sr_message_t msg) {

}

void OpenGLESRender::surfaceDestroyed(sr_message_t msg) {
    LOGD("OpenGLESRender::surfaceDestroyed enter\n");
    gl_renderer_remove_window(renderer);
    LOGD("OpenGLESRender::surfaceDestroyed exit\n");
}

