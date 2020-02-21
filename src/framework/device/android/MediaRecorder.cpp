//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include <VideoEncoder.h>
#include "MediaRecorder.h"
#include "MediaContext.h"


using namespace freee;


enum {
    RecvMsg_None = 0,
    RecvMsg_Open = 1,
    RecvMsg_Start = 2,
    RecvMsg_Stop = 3,
    RecvMsg_Close = 4,
    RecvMsg_StartRecord = 5,
    RecvMsg_StopRecord = 6,
    RecvMsg_StartPreview = 7,
    RecvMsg_StopPreview = 8,
    RecvMsg_UpdateConfig = 9,
};


enum {
    Status_Closed = 0,
    Status_Opened = 1,
    Status_Started = 2,
    Status_Stopped = 3,
};


MediaRecorder::MediaRecorder(){
    videoSource = nullptr;
//    audioSource = nullptr;
    mStatus = Status_Closed;
    SetContextName("MediaRecorder");
    StartProcessor("MediaRecorder");
}

MediaRecorder::~MediaRecorder() {
    SendMessage(MediaPacket(RecvMsg_Close));
    StopProcessor();
}

void MediaRecorder::ProcessPacket(MediaPacket pkt) {
    switch (pkt.msg.key){
        case RecvMsg_Open:
            Open(pkt);
            break;
        case RecvMsg_Close:
            Close();
            break;
        case RecvMsg_StartRecord:
            StartRecord();
            break;
        case RecvMsg_StopRecord:
            StopRecord();
            break;
        case RecvMsg_StartPreview:
            StartPreview(pkt);
            break;
        case RecvMsg_StopPreview:
            StopPreview();
            break;
        case RecvMsg_Start:
            Start();
            break;
        case RecvMsg_Stop:
            Stop();
            break;
        case RecvMsg_UpdateConfig:
            break;
        default:
            break;
    }
}

void MediaRecorder::onReceiveMessage(MediaPacket pkt) {
    ProcessPacket(pkt);
}

void MediaRecorder::Open(MediaPacket pkt) {
    if (mStatus == Status_Closed){

        mConfig = json::parse(pkt.msg.json);
        videoSource = new MyVideoSource();
        videoSource->Open(mConfig["video"]);

        mStatus = Status_Opened;
    }
}

void MediaRecorder::Close() {
    if (mStatus == Status_Started){
        videoSource->Stop();
    }
    if (mStatus == Status_Opened
        || mStatus == Status_Stopped){

        videoSource->Close();
        delete videoSource;

        mStatus = Status_Closed;
    }
}

void MediaRecorder::StartRecord() {
    if (!isRecording){
        if (mStatus != Status_Started){
            Start();
        }
        if (mStatus == Status_Started){
//            videoSource->SetEncoder(videoEncoder);
            isRecording = true;
        }
    }
}

void MediaRecorder::StopRecord() {
    if (isRecording){
        isRecording = false;
//        videoSource->SetEncoder(nullptr);
        if (!isPreviewing){
            Stop();
        }
    }
}

void MediaRecorder::StartPreview(MediaPacket pkt) {
    if (!isPreviewing){
        if (mStatus != Status_Started){
            Start();
        }
        if (mStatus == Status_Started){
            videoSource->SetWindow((MessageContext*)pkt.msg.ptr);
            videoSource->StartPreview();
            isPreviewing = true;
        }
    }
}

void MediaRecorder::StopPreview() {
    if (isPreviewing){
        isPreviewing = false;
        videoSource->StopPreview();
        if (!isRecording){
            Stop();
        }
    }
}

void MediaRecorder::Start() {
    if (mStatus != Status_Started){
        if (mStatus == Status_Opened || mStatus == Status_Stopped){

            videoSource->Start();

            mStatus = Status_Started;
        }
    }
}

void MediaRecorder::Stop() {
    if (mStatus == Status_Started){

        videoSource->Stop();

        mStatus = Status_Stopped;
    }
}

