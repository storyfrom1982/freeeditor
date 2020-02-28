//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include <VideoEncoder.h>
#include <BufferPool.h>
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
        : MediaChainImpl(MediaType_All, MediaNumber_Recorder, "MediaRecorder"),
        mAudioSource(nullptr),
        mVideoSource(nullptr),
        mVideoFilter(nullptr),
        mVideoRenderer(nullptr),
        mVideoEncoder(nullptr),
        isPreviewing(false),
        mStatus(Status_Closed){
    SetContextName("MediaRecorder");
    StartProcessor("MediaRecorder");
}

MediaRecorder::~MediaRecorder() {
    ProcessMessage(SmartPkt(RecvMsg_Close));
    StopProcessor();
//    FinalClear();
}

void MediaRecorder::FinalClear() {
    if (mAudioSource){
        delete mAudioSource;
        mAudioSource = nullptr;
    }
    if (mVideoSource){
        delete mVideoSource;
        mVideoSource = nullptr;
    }
    if (mVideoEncoder){
        delete mVideoEncoder;
        mVideoEncoder = nullptr;
    }
    if (mVideoRenderer){
        delete mVideoRenderer;
        mVideoRenderer = nullptr;
    }
    if (mVideoFilter){
        delete mVideoFilter;
        mVideoFilter = nullptr;
    }
}

void MediaRecorder::MessageProcess(SmartPkt pkt) {
    switch (pkt.GetKey()){
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
        case (MediaNumber_VideoSource + Status_Stopped):
            FinalClearVideoChain();
            break;
        default:
            break;
    }
}

void MediaRecorder::onRecvMessage(SmartPkt pkt) {
    ProcessMessage(SmartPkt(pkt));
}

void MediaRecorder::Open(SmartPkt pkt) {
    if (mStatus == Status_Closed){

        mConfig = json::parse(pkt.GetString());

        LOGD("MediaRecorder config >> %s\n", mConfig.dump(4).c_str());

        mVideoSource = new VideoSource();
        mVideoFilter = new VideoFilter();
        mVideoRenderer = new VideoRenderer();
        mVideoEncoder = VideoEncoder::Create(mConfig["video"]["codecName"]);

        mVideoSource->SetEventCallback(this);

        mVideoSource->AddOutputChain(mVideoFilter);
        mVideoFilter->AddOutputChain(mVideoRenderer);
        mVideoFilter->AddOutputChain(mVideoEncoder);

        mVideoSource->Open(this);

        mAudioSource = new AudioSource();
        mAudioSource->Open(this);

        mStatus = Status_Opened;
    }
}

void MediaRecorder::Close() {
    if (mStatus == Status_Started){
        Stop();
    }
    if (mStatus == Status_Opened
        || mStatus == Status_Stopped){

        mVideoSource->RemoveOutputChain(mVideoFilter);
        mVideoFilter->RemoveOutputChain(mVideoRenderer);
        mVideoFilter->RemoveOutputChain(mVideoEncoder);

        mVideoSource->Close(this);
        mVideoFilter->Close(this);
        mVideoEncoder->Close(this);
        mVideoRenderer->Close(this);

        delete mVideoRenderer;
        delete mVideoEncoder;
        delete mVideoSource;
        delete mVideoFilter;

        mAudioSource->Close(this);

        delete mAudioSource;

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
            mVideoRenderer->SetVideoWindow(pkt.GetPtr());
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
        if (mStatus == Status_Opened
            || mStatus == Status_Stopped){
            mVideoSource->Start(this);
            mAudioSource->Start(this);
            mStatus = Status_Started;
        }
    }
}

void MediaRecorder::Stop() {
    if (mStatus == Status_Started){
        mVideoSource->Stop(this);
        mAudioSource->Stop(this);
        mStatus = Status_Stopped;
    }
}

json &MediaRecorder::GetConfig(MediaChain *chain) {
    if (chain->GetType(this) == MediaType_Video){
        return mConfig["video"];
    }else if (chain->GetType(this) == MediaType_Audio){
        return mConfig["audio"];
    }
    return MediaChainImpl::GetConfig(chain);
}

void MediaRecorder::onEvent(MediaChain *chain, SmartPkt pkt) {
    LOGD("MediaRecorder::onEvent[%d]\n", pkt.GetKey());
    ProcessMessage(pkt);
}

void MediaRecorder::FinalClearVideoChain() {

}

