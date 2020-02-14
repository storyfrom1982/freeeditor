//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include <VideoEncoder.h>
#include "MediaRecorder.h"
#include "MediaContext.h"


using namespace freee;

MediaRecorder::MediaRecorder(){
    videoSource = NULL;
    SetContextName("MediaRecorder");
    StartMessageProcessor();
}

MediaRecorder::~MediaRecorder() {
    LOGD("MediaRecorder::~MediaRecorder");
    if (videoSource){
        delete videoSource;
    }
    if (videoEncoder){
        delete videoEncoder;
    }
}

void MediaRecorder::MessageProcessor(sr_message_t msg) {
    switch (msg.key){
        case Record_SetConfig:
            Initialize(msg);
            break;
        case Record_StartCapture:
            videoSource->Start();
            break;
        case Record_StartPreview:
            StartPreview(msg);
            break;
        case Record_DrawPicture:
            break;
        default:
            break;
    }
}

void MediaRecorder::Initialize(sr_message_t msg) {
    mConfig = json::parse(msg.str);
    videoSource = new VideoSource();
    videoSource->Open(mConfig["video"]);
    videoEncoder = new VideoEncoder;
    videoSource->SetEncoder(videoEncoder);
}

void MediaRecorder::StartPreview(sr_message_t msg) {
    videoSource->SetWindow((MessageContext*)msg.ptr);
    videoSource->StartPreview();
}

void MediaRecorder::OnPutMessage(sr_message_t msg) {
    ProcessMessage(msg);
}

