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
        : MessageChain(MediaType_All, MediaNumber_Recorder, "MediaRecorder"),
        m_audioSource(nullptr),
        m_audioFilter(nullptr),
        m_audioEncoder(nullptr),
        m_videoSource(nullptr),
        m_videoFilter(nullptr),
        m_videoRenderer(nullptr),
        m_videoEncoder(nullptr),
        m_mediaStream(nullptr),
        is_previewing(false),
        m_status(Status_Closed){
    SetContextName("MediaRecorder");
    StartProcessor("MediaRecorder");
}

MediaRecorder::~MediaRecorder() {
    ProcessMessage(Message(MsgKey_Close));
    StopProcessor();
//    FinalClear();
}

void MediaRecorder::FinalClear() {
    if (m_audioSource){
        delete m_audioSource;
        m_audioSource = nullptr;
    }
    if (m_videoSource){
        delete m_videoSource;
        m_videoSource = nullptr;
    }
    if (m_videoEncoder){
        delete m_videoEncoder;
        m_videoEncoder = nullptr;
    }
    if (m_videoRenderer){
        delete m_videoRenderer;
        m_videoRenderer = nullptr;
    }
    if (m_videoFilter){
        delete m_videoFilter;
        m_videoFilter = nullptr;
    }
}

void MediaRecorder::MessageProcess(Message pkt) {
    switch (pkt.GetKey()){
        case MsgKey_Open:
            Open(pkt);
            break;
        case MsgKey_Close:
            Close();
            break;
        case RecvMsg_StartRecord:
            StartRecord(pkt);
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
        case MsgKey_Start:
            Start();
            break;
        case MsgKey_Stop:
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

void MediaRecorder::onRecvMessage(Message pkt) {
    ProcessMessage(Message(pkt));
}

void MediaRecorder::Open(Message pkt) {
    if (m_status == Status_Closed){

        m_config = json::parse(pkt.GetString());

        LOGD("MediaRecorder config >> %s\n", m_config.dump(4).c_str());

        m_mediaStream = MediaStream::Create("file");
        m_mediaStream->ConnectStream("/storage/emulated/0/test.mp4");

        m_videoSource = new VideoSource();
        m_videoFilter = new VideoFilter();
        m_videoRenderer = new VideoRenderer();
        m_videoEncoder = VideoEncoder::Create(m_config["video"]["codecName"]);

//        m_videoSource->SetEventCallback(this);

        m_videoSource->AddOutput(m_videoFilter);
        m_videoFilter->AddOutput(m_videoRenderer);
        m_videoFilter->AddOutput(m_videoEncoder);
        m_videoEncoder->AddOutput(m_mediaStream);
        m_videoSource->Open(this);


        m_audioSource = new AudioSource();
        m_audioFilter = new AudioFilter();
        m_audioEncoder = AudioEncoder::Create(m_config["audio"]["codecName"]);
        m_audioSource->AddOutput(m_audioFilter);
        m_audioFilter->AddOutput(m_audioEncoder);
        m_audioEncoder->AddOutput(m_mediaStream);
        m_audioSource->Open(this);



        m_status = Status_Opened;
    }
}

void MediaRecorder::Close() {
    if (m_status == Status_Started){
        Stop();
    }
    if (m_status == Status_Opened
        || m_status == Status_Stopped){

        m_videoSource->DelOutput(m_videoFilter);
        m_videoFilter->DelOutput(m_videoRenderer);
        m_videoFilter->DelOutput(m_videoEncoder);
        m_videoEncoder->DelOutput(m_mediaStream);

        m_videoSource->Close(this);
        m_videoFilter->Close(this);
        m_videoEncoder->Close(this);
        m_videoRenderer->Close(this);

        delete m_videoRenderer;
        delete m_videoEncoder;
        delete m_videoSource;
        delete m_videoFilter;


        m_audioSource->DelOutput(m_audioFilter);
        m_audioFilter->DelOutput(m_audioEncoder);
        m_audioEncoder->DelOutput(m_mediaStream);
        m_audioSource->Close(this);
        m_audioFilter->Close(this);
        m_audioEncoder->Close(this);

        m_mediaStream->DisconnectStream();


        delete m_audioSource;
        delete m_audioFilter;
        delete m_audioEncoder;

        delete m_mediaStream;

        m_status = Status_Closed;
    }
}

void MediaRecorder::StartRecord(Message pkt) {
    LOGD("MediaRecorder::StartRecord url=%s\n", pkt.GetString().c_str());
    if (!is_recording){
        if (m_status != Status_Started){
            Start();
        }
        if (m_status == Status_Started){
            is_recording = true;
        }
    }
}

void MediaRecorder::StopRecord() {
    if (is_recording){
        is_recording = false;
        if (!is_previewing){
            Stop();
        }
    }
}

void MediaRecorder::StartPreview(Message pkt) {
    LOGD("MediaRecorder::StartPreview enter\n");
    if (!is_previewing){
        if (m_status != Status_Started){
            Start();
        }
        if (m_status == Status_Started){
            m_videoRenderer->SetVideoWindow(pkt.GetPtr());
            is_previewing = true;
        }
    }
    LOGD("MediaRecorder::StartPreview exit\n");
}

void MediaRecorder::StopPreview() {
    if (is_previewing){
        is_previewing = false;

        if (!is_recording){
            Stop();
        }
    }
}

void MediaRecorder::Start() {
    if (m_status != Status_Started){
        if (m_status == Status_Opened
            || m_status == Status_Stopped){
            m_videoSource->Start(this);
            m_audioSource->Start(this);
            m_status = Status_Started;
        }
    }
}

void MediaRecorder::Stop() {
    if (m_status == Status_Started){
        m_videoSource->Stop(this);
        m_audioSource->Stop(this);
        m_status = Status_Stopped;
    }
}

json &MediaRecorder::GetConfig(MessageChain *chain) {
    if (chain->GetType(this) == MediaType_Video){
        return m_config["video"];
    }else if (chain->GetType(this) == MediaType_Audio){
        return m_config["audio"];
    }
    return MessageChain::GetConfig(chain);
}

//void MediaRecorder::onEvent(MediaChain *chain, SmartPkt pkt) {
//    LOGD("MediaRecorder::onEvent[%d]\n", pkt.GetKey());
//    ProcessMessage(pkt);
//}

void MediaRecorder::FinalClearVideoChain() {

}

void MediaRecorder::ConnectContext(MessageContext *context) {
    MessageContext::ConnectContext(context);
}

