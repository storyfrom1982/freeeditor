//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include "MediaRecord.h"
#include "AndroidDevice.h"


using namespace freee;

MediaRecord::MediaRecord(DeviceContext *ctx) : DeviceContext(ctx) {
    m_processor.name = "MediaRecord";
    m_processor.handler = this;
    m_processor.process = MediaRecord::messageProcessorThread;
    m_queue = sr_msg_queue_create();
    sr_msg_queue_start_processor(m_queue, &m_processor);
}

MediaRecord::~MediaRecord() {
    LOGD("MediaRecord::~MediaRecord");
    videoSource->closeSource();
    sr_msg_queue_complete(m_queue);
    sr_msg_queue_remove(&m_queue);
}

int MediaRecord::onPutObject(int type, void *obj) {
    LOGD("MediaRecord::onPutObject:: %p", obj);
    window = new NativeWindow(obj);
    glesRender->onPutObject(OpenGLESRender_SetSurfaceView, window);
    return 0;
}

void *MediaRecord::onGetObject(int type) {
    return nullptr;
}

int MediaRecord::onPutMessage(int cmd, std::string msg) {
    sr_msg_t ms;
    ms.key = cmd;
    ms.size = msg.length();
    ms.ptr = strdup(msg.c_str());
    sr_msg_queue_push(m_queue, ms);
    return 0;
}

std::string MediaRecord::onGetMessage(int cmd) {
    return std::string();
}

int MediaRecord::onPutData(void *data, int size) {
    VideoPacket nv21Pkt = {0};
    videoPacket_FillData(&nv21Pkt, (uint8_t*)data, 640, 360, libyuv::FOURCC_NV21);

    VideoPacket *y420Pkt;
    y420Pkt = videoPacket_Alloc(360, 640, libyuv::FOURCC_I420);
    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, 270);
    glesRender->onPutObject(OpenGLESRender_DrawPicture, y420Pkt);
//    LOGD("camera data size: %d", size);
    return 0;
}

void *MediaRecord::onGetBuffer() {
    return nullptr;
}

void MediaRecord::messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg) {
    switch (msg.key){
        case Record_SetConfig:
            init(msg);
            break;
        case Record_StartCapture:
            videoSource->startCapture();
            break;
        default:
            break;
    }
}

void MediaRecord::messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg) {
    static_cast<MediaRecord *>(processor->handler)->messageProcessorLoop(processor, msg);
}

void MediaRecord::init(sr_msg_t msg) {
    json cfg = json::parse((char*)msg.ptr);
    LOGD("camera %s", cfg["videoSource"].dump().c_str());
    DeviceContext *context = (DeviceContext*) AndroidDevice::Instance()->getObject(0);
    videoSource = VideoSource::openVideoSource(context);
    videoSource->openSource(cfg["videoSource"]);
    videoSource->setEncoder(this);
    glesRender = new OpenGLESRender(NULL);
    glesRender->onPutMessage(OpenGLESRender_Init, "");

}
