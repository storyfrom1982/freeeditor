//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include "MediaRecord.h"
#include "MediaContext.h"


using namespace freee;

MediaRecord::MediaRecord(){
    glesRender = NULL;
    videoSource = NULL;
    SetContextName("MediaRecord");
    StartMessageProcessor();
}

MediaRecord::~MediaRecord() {
    LOGD("MediaRecord::~MediaRecord");
    if (videoSource){
        videoSource->closeSource();
        delete videoSource;
    }
    if (glesRender){
        delete glesRender;
    }
//    sr_message_queue_clear(m_queue);
//    sr_message_queue_release(&m_queue);
}

//int MediaRecord::onPutData(void *data, int size) {
//    VideoPacket nv21Pkt = {0};
//    videoPacket_FillData(&nv21Pkt, (uint8_t*)data, 640, 360, libyuv::FOURCC_NV21);
//
//    VideoPacket *y420Pkt;
//    y420Pkt = videoPacket_Alloc(360, 640, libyuv::FOURCC_I420);
//    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, 270);
//    glesRender->onPutObject(OpenGLESRender_DrawPicture, y420Pkt);
////    LOGD("camera data size: %d", size);
//    return 0;
//}

//void MediaRecord::OnPutMessage(sr_message_t msg) {
////    sr_message_queue_put(m_queue, msg);
//    ProcessorMessage(msg);
//}

sr_message_t MediaRecord::OnGetMessage(sr_message_t msg) {
    return sr_message_t();
}

void MediaRecord::MessageProcessor(sr_message_t msg) {
    switch (msg.key){
        case Record_SetConfig:
            init(msg);
            break;
        case Record_StartCapture:
            videoSource->startCapture();
            break;
        case Record_StartPreview:
            StartPreview(msg);
            break;
        case Record_DrawPicture:
            drawPicture(msg);
            break;
        default:
            break;
    }
}

//void MediaRecord::messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg) {
//    static_cast<MediaRecord *>(processor->handler)->MessageProcessor(msg);
//}

void MediaRecord::init(sr_message_t msg) {
    json cfg = json::parse((char*)msg.ptr);
//    LOGD("camera %s", cfg["videoSource"].dump().c_str());
    msg.key = MediaMessage_CreateCamera;
    msg.size = MessageType_Pointer;
    msg = MediaContext::Instance()->GetMessage(msg);
    MessageContext *context = (MessageContext*) msg.ptr;
    videoSource = VideoSource::openVideoSource(context);
    videoSource->openSource(cfg["videoSource"]);
    videoSource->setEncoder(this);
    glesRender = new OpenGLESRender();
    msg.key = OpenGLESRender_Init;
    glesRender->OnPutMessage(msg);
}

void MediaRecord::StartPreview(sr_message_t msg) {
    msg.key = OpenGLESRender_SetSurfaceView;
    glesRender->OnPutMessage(msg);

}

void MediaRecord::drawPicture(sr_message_t msg) {
    VideoPacket nv21Pkt = {0};
    videoPacket_FillData(&nv21Pkt, (uint8_t*)msg.ptr, 640, 360, libyuv::FOURCC_NV21);

    VideoPacket *y420Pkt;
    y420Pkt = videoPacket_Alloc(360, 640, libyuv::FOURCC_I420);
    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, 270);
    msg.key = OpenGLESRender_DrawPicture;
    msg.ptr = y420Pkt;
    glesRender->OnPutMessage(msg);
}

void MediaRecord::OnPutDataBuffer(sr_message_t msg) {
    drawPicture(msg);
}

