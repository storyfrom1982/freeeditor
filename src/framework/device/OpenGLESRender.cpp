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


OpenGLESRender::OpenGLESRender(DeviceContext *ctx)
    : DeviceContext(ctx) {
    m_processor.name = "OpenGLESRender";
    m_processor.handler = this;
    m_processor.process = OpenGLESRender::messageProcessorThread;
    m_queue = sr_msg_queue_create();
    sr_msg_queue_start_processor(m_queue, &m_processor);
}

OpenGLESRender::~OpenGLESRender() {

}

int OpenGLESRender::onPutObject(int type, void *obj) {
//    LOGD("onPutObject data size: %d", type);
    sr_msg_t msg;
    msg.key = type;
    msg.ptr = obj;
    msg.size = 0;
    sr_msg_queue_push(m_queue, msg);
    return 0;
}

void *OpenGLESRender::onGetObject(int type) {
    return nullptr;
}

int OpenGLESRender::onPutMessage(int cmd, std::string str) {
    sr_msg_t msg;
    msg.key = cmd;
    msg.size = str.length();
    msg.ptr = strdup(str.c_str());
    sr_msg_queue_push(m_queue, msg);
    return 0;
}

std::string OpenGLESRender::onGetMessage(int cmd) {
    return std::string();
}

int OpenGLESRender::onPutData(void *data, int size) {
    return 0;
}

void *OpenGLESRender::onGetBuffer() {
    return nullptr;
}

void OpenGLESRender::messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg) {

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

void OpenGLESRender::messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg) {
    ((OpenGLESRender*)processor->handler)->messageProcessorLoop(processor, msg);
}

void OpenGLESRender::init(sr_msg_t msg) {
    renderer = gl_renderer_create(16, 16);
    opengles_open(&opengles);
}

void OpenGLESRender::setSurfaceView(sr_msg_t msg) {
    NativeWindow *window = (NativeWindow*)msg.ptr;
    gl_renderer_set_window(renderer, (gl_window_t*)window->getWindowHandler());
    int w, h;
    window->getWindowSize(&w, &h);
    glViewport(0, 0, w, h);
}

void OpenGLESRender::drawPicture(sr_msg_t msg) {
//    LOGD("drawPicture data size: %d", msg.key);
    opengles_render(opengles, (const VideoPacket*)msg.ptr);
    gl_renderer_swap_buffers(renderer);
}

