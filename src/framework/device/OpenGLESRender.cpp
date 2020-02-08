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


OpenGLESRender::OpenGLESRender(MessageContext *ctx) {
    SetContextHandler(ctx);
//    m_processor.name = "OpenGLESRender";
//    m_processor.handler = this;
//    m_processor.process = OpenGLESRender::messageProcessorThread;
//    m_queue = sr_message_queue_create();
//    sr_message_queue_start_processor(m_queue, &m_processor);
    SetContextName("OpenGLESRender");
    StartMessageProcessor();
}

OpenGLESRender::~OpenGLESRender() {
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
    gl_renderer_set_window(renderer, (gl_window_t*)window->getWindowHandler());
    int w, h;
    window->getWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void OpenGLESRender::drawPicture(sr_message_t msg) {
//    LOGD("drawPicture data size: %d", msg.key);
    opengles_render(opengles, (const VideoPacket*)msg.ptr);
    gl_renderer_swap_buffers(renderer);
    videoPacket_Free(reinterpret_cast<VideoPacket **>(&msg.ptr));
}

