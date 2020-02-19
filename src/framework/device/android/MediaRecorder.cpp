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
    audioSource = NULL;
    StartProcessor("MediaRecorder");
}

MediaRecorder::~MediaRecorder() {
    LOGD("MediaRecorder::~MediaRecorder");
    if (videoSource){
        delete videoSource;
    }
    if (videoEncoder){
        delete videoEncoder;
    }
    if (audioSource){
        delete audioSource;
    }
}

void MediaRecorder::MessageProcessor(SrPkt pkt) {
    switch (pkt.msg.key){
        case Record_SetConfig:
            Initialize(pkt);
            break;
        case Record_StartCapture:
            videoSource->Start();
            audioSource->Start();
            break;
        case Record_StartPreview:
            StartPreview(pkt);
            break;
        case Record_DrawPicture:
            break;
        case Record_SetUrl:
            mediaProtocol = MediaProtocol::Create(pkt.msg.js);
            audioEncoder->SetProtocol(mediaProtocol);
            videoEncoder->SetProtocol(mediaProtocol);
            break;
        default:
            break;
    }
}

void MediaRecorder::Initialize(SrPkt pkt) {
    mConfig = json::parse(pkt.msg.js);
    videoSource = new VideoSource();
    videoEncoder = VideoEncoder::Create("x264");
    videoSource->SetEncoder(videoEncoder);
    videoSource->Open(mConfig["video"]);

    audioSource = new AudioSource();
    audioEncoder = AudioEncoder::Create("aac");
    audioSource->SetEncoder(audioEncoder);
    audioSource->Open(mConfig["audio"]);

}

void MediaRecorder::StartPreview(SrPkt pkt) {
    videoSource->SetWindow((MessageContext*)pkt.msg.ptr);
    videoSource->StartPreview();
}

void MediaRecorder::onReceiveMessage(SrPkt msg) {
    PutMessage(msg);
}

