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


MediaRecorder::MediaRecorder()
        : MediaChainImpl(MediaType_All, MediaNumber_Recorder, "MediaRecorder") {
    mVideoSource = nullptr;
    mVideoRenderer = nullptr;
    isPreviewing = false;
//    audioSource = nullptr;
    mStatus = Status_Closed;
    SetContextName("MediaRecorder");
    StartProcessor("MediaRecorder");
}

MediaRecorder::~MediaRecorder() {
    SendMessage(SmartPkt(RecvMsg_Close));
    StopProcessor();
}

void MediaRecorder::ProcessMessage(SmartPkt pkt) {
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

void MediaRecorder::onRecvMessage(SmartPkt pkt) {
    ProcessMessage(pkt);
}

void MediaRecorder::Open(SmartPkt pkt) {
    if (mStatus == Status_Closed){

        mMediaConfig = json::parse(pkt.msg.json);

        mVideoSource = new VideoSource();
        mVideoSource->Open(this);

        mVideoRenderer = new VideoRenderer();
        mVideoSource->AddOutputChain(mVideoRenderer);

//        videoSource = new MyVideoSource();
//        videoSource->Open(mMediaConfig["video"]);

        mStatus = Status_Opened;
    }
}

void MediaRecorder::Close() {
    if (mStatus == Status_Started){
        Stop();
    }
    if (mStatus == Status_Opened
        || mStatus == Status_Stopped){

        mVideoSource->RemoveOutputChain(mVideoRenderer);

        mVideoRenderer->Close(this);
        delete mVideoRenderer;

        mVideoSource->Close(this);
        delete mVideoSource;

        mStatus = Status_Closed;
    }
}

void MediaRecorder::StartRecord() {
    if (!isRecording){
        if (mStatus != Status_Started){
            Start();
        }
        if (mStatus == Status_Started){
            isRecording = true;
        }
    }
}

void MediaRecorder::StopRecord() {
    if (isRecording){
        isRecording = false;
        if (!isPreviewing){
            Stop();
        }
    }
}

void MediaRecorder::StartPreview(SmartPkt pkt) {
    LOGD("MediaRecorder::StartPreview enter\n");
    if (!isPreviewing){
        LOGD("MediaRecorder::StartPreview 1\n");
        if (mStatus != Status_Started){
            Start();
        }
        if (mStatus == Status_Started){
            LOGD("MediaRecorder::StartPreview 2\n");
            mVideoRenderer->SetVideoWindow(pkt);
            isPreviewing = true;
        }
    }
    LOGD("MediaRecorder::StartPreview exit\n");
}

void MediaRecorder::StopPreview() {
    if (isPreviewing){
        isPreviewing = false;

        if (!isRecording){
            Stop();
        }
    }
}

void MediaRecorder::Start() {
    if (mStatus != Status_Started){
        if (mStatus == Status_Opened || mStatus == Status_Stopped){

            mVideoSource->Start(this);

            mStatus = Status_Started;
        }
    }
}

void MediaRecorder::Stop() {
    if (mStatus == Status_Started){

        mVideoSource->Stop(this);

        mStatus = Status_Stopped;
    }
}

json &MediaRecorder::GetMediaConfig(MediaChain *chain) {
    if (chain->GetMediaType(this) == MediaType_Video){
        return mMediaConfig["video"];
    }
    return MediaChainImpl::GetMediaConfig(chain);
}

