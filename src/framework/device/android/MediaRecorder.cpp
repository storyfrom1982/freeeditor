//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include <VideoEncoder.h>
#include "MediaRecorder.h"
#include "MediaContext.h"


using namespace freee;

MediaRecorder::MediaRecorder(){
    glesRender = NULL;
    videoSource = NULL;
    SetContextName("MediaRecorder");
    StartMessageProcessor();
}

MediaRecorder::~MediaRecorder() {
    LOGD("MediaRecorder::~MediaRecorder");
    if (videoSource){
        videoSource->Close();
        delete videoSource;
    }
    if (glesRender){
        delete glesRender;
    }
    if (encoder){
        delete encoder;
    }
//    sr_message_queue_clear(m_queue);
//    sr_message_queue_release(&m_queue);
}

//int MediaRecorder::onPutData(void *data, int size) {
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

//void MediaRecorder::OnPutMessage(sr_message_t msg) {
////    sr_message_queue_put(m_queue, msg);
//    ProcessorMessage(msg);
//}

sr_message_t MediaRecorder::OnGetMessage(sr_message_t msg) {
    return sr_message_t();
}

void MediaRecorder::MessageProcessor(sr_message_t msg) {
    switch (msg.key){
        case Record_SetConfig:
            init(msg);
            break;
        case Record_StartCapture:
            videoSource->Start();
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

//void MediaRecorder::messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg) {
//    static_cast<MediaRecorder *>(processor->handler)->MessageProcessor(msg);
//}

void MediaRecorder::init(sr_message_t msg) {
    json cfg = json::parse(msg.str);
    __sr_msg_clear(msg);
//    LOGD("camera %s", cfg["videoSource"].dump().c_str());
//    MessageContext *camera = MediaContext::Instance()->CreateCamera();
    videoSource = new VideoSource();
    videoSource->Open(cfg["videoSource"]);
    encoder = new VideoEncoder;
    videoSource->SetEncoder(encoder);
//    glesRender = new OpenGLESRender();
//    msg.key = OpenGLESRender_Init;
//    glesRender->OnPutMessage(msg);
}

void MediaRecorder::StartPreview(sr_message_t msg) {
//    msg.key = OpenGLESRender_SetSurfaceView;
//    glesRender->OnPutMessage(msg);
    videoSource->SetWindow((MessageContext*)msg.ptr);
    videoSource->StartPreview();
}

void MediaRecorder::drawPicture(sr_message_t msg) {
    VideoPacket nv21Pkt = {0};
    videoPacket_FillData(&nv21Pkt, (uint8_t*)msg.ptr, 640, 360, libyuv::FOURCC_NV21);

    VideoPacket *y420Pkt;
    y420Pkt = videoPacket_Alloc(360, 640, libyuv::FOURCC_I420);
    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, 270);
    msg.key = OpenGLESRender_DrawPicture;
    msg.ptr = y420Pkt;
    glesRender->OnPutMessage(msg);
}

void MediaRecorder::OnPutDataBuffer(sr_message_t msg) {
    drawPicture(msg);
}

