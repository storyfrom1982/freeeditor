//
// Created by yongge on 20-2-3.
//

#include <MConfig.h>
#include <VideoEncoder.h>
#include <MessagePool.h>
#include "MediaRecorder.h"
#include "MediaContext.h"


using namespace freee;


enum {
    MsgKey_UpdateConfig = 11,
    MsgKey_StartPreview = 12,
    MsgKey_StopPreview = 13,
    MsgKey_StartRecord = 14,
    MsgKey_StopRecord = 15,
};


MediaRecorder::MediaRecorder()
        : MessageChain("MediaRecorder", MediaType_All),
        m_audioSource(nullptr),
        m_audioFilter(nullptr),
        m_audioEncoder(nullptr),
        m_videoSource(nullptr),
        m_videoFilter(nullptr),
        m_videoRenderer(nullptr),
        m_videoEncoder(nullptr),
        m_mediaStream(nullptr),
        is_previewing(false){
    StartProcessor();
}

MediaRecorder::~MediaRecorder() {
    ProcessMessage(NewMessage(MsgKey_Close));
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

void MediaRecorder::onRecvMessage(Message msg) {
    ProcessMessage(msg);
}

void MediaRecorder::onMsgStartRecord(Message pkt) {
    LOGD("MediaRecorder::StartRecord url=%s\n", pkt.GetString().c_str());
    if (!is_recording){
        if (m_status != Status_Started){
            onMsgStart(pkt);
        }
        if (m_status == Status_Started){
            m_mediaStream->ConnectStream("/storage/emulated/0/test.mp4");
            is_recording = true;
        }
    }
}

void MediaRecorder::onMsgStopRecord(Message msg) {
    if (is_recording){
        is_recording = false;
        m_videoFilter->DelOutput(m_videoEncoder);
        m_audioFilter->DelOutput(m_audioEncoder);
        m_videoEncoder->DelOutput(m_mediaStream);
        m_audioEncoder->DelOutput(m_mediaStream);
        m_mediaStream->DisconnectStream();
        if (!is_previewing){
            onMsgStop(msg);
        }
    }
}

void MediaRecorder::onMsgStartPreview(Message pkt) {
    LOGD("MediaRecorder::StartPreview enter\n");
    if (!is_previewing){
        if (m_status != Status_Started){
            onMsgStart(pkt);
        }
        if (m_status == Status_Started){
            m_videoFilter->AddOutput(m_videoRenderer);
            m_videoRenderer->SetVideoWindow(pkt.GetObjectPtr());
            is_previewing = true;
        }
    }
    LOGD("MediaRecorder::StartPreview exit\n");
}

void MediaRecorder::onMsgStopPreview(Message msg) {
    if (is_previewing){
        is_previewing = false;
        m_videoFilter->DelOutput(m_videoRenderer);
        if (!is_recording){
            onMsgStop(msg);
        }
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

void MediaRecorder::onMsgOpen(Message pkt) {
    if (m_status == Status_Closed){

        m_config = json::parse(pkt.GetString());

        LOGD("MediaRecorder config >> %s\n", m_config.dump(4).c_str());

        m_mediaStream = MediaStream::Create("file");
        m_mediaStream->SetEventListener(this);

        m_videoSource = new VideoSource();
        m_videoFilter = new VideoFilter();
        m_videoRenderer = new VideoRenderer();
        m_videoEncoder = VideoEncoder::Create(m_config["video"]["codecName"]);

        m_videoSource->AddOutput(m_videoFilter);
        m_videoSource->Open(this);


        m_audioSource = new AudioSource();
        m_audioFilter = new AudioFilter();
        m_audioEncoder = AudioEncoder::Create(m_config["audio"]["codecName"]);

        m_audioSource->AddOutput(m_audioFilter);
        m_audioSource->Open(this);



        m_status = Status_Opened;
    }
}

void MediaRecorder::onMsgClose(Message pkt) {
    if (m_status == Status_Started){
        onMsgStop(pkt);
    }
    if (m_status == Status_Opened
        || m_status == Status_Stopped){

        if (is_recording){
            onMsgStopRecord(pkt);
        }
        if (is_previewing){
            onMsgStopPreview(pkt);
        }

        LOGD("MediaRecorder::onMsgClose close video\n");
        m_videoSource->DelOutput(m_videoFilter);

        m_videoSource->Close(this);
        m_videoFilter->Close(this);
        m_videoEncoder->Close(this);
        m_videoRenderer->Close(this);


        LOGD("MediaRecorder::onMsgClose close audio\n");
        m_audioSource->DelOutput(m_audioFilter);

        m_audioSource->Close(this);
        m_audioFilter->Close(this);
        m_audioEncoder->Close(this);

        delete m_videoRenderer;
        delete m_videoEncoder;
        delete m_videoSource;
        delete m_videoFilter;

        delete m_audioSource;
        delete m_audioFilter;
        delete m_audioEncoder;

        delete m_mediaStream;

        m_status = Status_Closed;
    }
}

void MediaRecorder::onMsgStart(Message pkt) {
    if (m_status != Status_Started){
        if (m_status == Status_Opened
            || m_status == Status_Stopped){
            m_videoSource->Start(this);
            m_audioSource->Start(this);
            m_status = Status_Started;
        }
    }
}

void MediaRecorder::onMsgStop(Message pkt) {
    if (m_status == Status_Started){
        m_videoSource->Stop(this);
        m_audioSource->Stop(this);
        m_status = Status_Stopped;
    }
}

void MediaRecorder::onMsgProcessEvent(Message pkt) {
    auto chain = pkt.GetObjectPtr();
    if (chain == m_mediaStream){
        if (pkt.event() == MsgKey_Open){
            LOGD("MediaStream Opened\n");
            if (is_recording){
                m_videoEncoder->AddOutput(m_mediaStream);
                m_audioEncoder->AddOutput(m_mediaStream);
                m_videoFilter->AddOutput(m_videoEncoder);
                m_audioFilter->AddOutput(m_audioEncoder);
                SendMessage(NewMessage(MsgKey_ProcessEvent, MsgKey_Open));
            }
        }
    }
}

void MediaRecorder::onMsgControl(Message pkt) {
    switch (pkt.key()){
        case MsgKey_StartRecord:
            onMsgStartRecord(pkt);
            break;
        case MsgKey_StopRecord:
            onMsgStopRecord(pkt);
            break;
        case MsgKey_StartPreview:
            onMsgStartPreview(pkt);
            break;
        case MsgKey_StopPreview:
            onMsgStopPreview(pkt);
            break;
        case MsgKey_UpdateConfig:
            break;
        default:
            break;
    }
}

