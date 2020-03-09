//
// Created by yongge on 20-2-15.
//

#include <MConfig.h>
#include <android/MediaContext.h>
#include "AudioSource.h"


using namespace freee;


enum {
    SendMsg_Open = 1,
    SendMsg_Close = 2,
    SendMsg_Start = 3,
    SendMsg_Stop = 4,
};

enum {
    OnRecvMsg_Opened = 1,
    OnRecvMsg_Started = 2,
    OnRecvMsg_Stopped = 3,
    OnRecvMsg_Closed = 4,
    OnRecvMsg_ProcessSound = 5
};


AudioSource::AudioSource(int mediaType, int mediaNumber, std::string mediaName)
        : MessageChainImpl(mediaType, mediaNumber, mediaName) {
//    MessageContext *context = MediaContext::Instance().ConnectMicrophone();
    MessageContext *context = MediaContext::Instance()->ConnectMicrophone();
    ConnectContext(context);
    SetContextName(mediaName);
}

AudioSource::~AudioSource() {
    Close(this);
    DisconnectContext();
//    MediaContext::Instance().DisconnectMicrophone();
    MediaContext::Instance()->DisconnectMicrophone();
    FinalClear();
}

void AudioSource::onRecvMessage(Message pkt) {
    switch (pkt.GetKey()){
        case OnRecvMsg_ProcessSound:
            ProcessData(this, pkt);
            break;
        case OnRecvMsg_Opened:
            m_status = Status_Opened;
            LOGD("AudioSource Opened\n");
            UpdateConfig(pkt);
            pkt.SetKey(MsgKey_Open);
            MessageChainImpl::onMsgOpen(pkt);
//            ReportEvent(SmartPkt(Status_Opened + m_number));
            break;
        case OnRecvMsg_Closed:
            m_status = Status_Closed;
//            CloseNext();
            pkt.SetKey(MsgKey_Close);
            MessageChainImpl::onMsgClose(pkt);
//            ReportEvent(SmartPkt(Status_Closed + m_number));
            FinalClear();
            LOGD("AudioSource Closed\n");
            break;
        case OnRecvMsg_Started:
            m_status = Status_Started;
//            ReportEvent(SmartPkt(Status_Started + m_number));
            LOGD("AudioSource Started\n");
            break;
        case OnRecvMsg_Stopped:
//            ReportEvent(SmartPkt(Status_Stopped + m_number));
            m_status = Status_Stopped;
            LOGD("AudioSource Stopped\n");
            break;
        default:
            break;
    }
}

Message AudioSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void AudioSource::Open(MessageChain *chain) {
    m_config = chain->GetConfig(this);
    SendMessage(NewJsonPkt(SendMsg_Open, m_config.dump()));
}

void AudioSource::Close(MessageChain *chain) {
    Message pkt(SendMsg_Close);
    SendMessage(pkt);
}

void AudioSource::Start(MessageChain *chain) {
    Message pkt(SendMsg_Start);
    SendMessage(pkt);
}

void AudioSource::Stop(MessageChain *chain) {
    Message pkt(SendMsg_Stop);
    SendMessage(pkt);
}

void AudioSource::ProcessData(MessageChain *chain, Message pkt) {
    Message resample = p_bufferPool->NewFrameMessage(MsgKey_ProcessData);
    memcpy(resample.GetDataPtr(), pkt.frame.data, resample.GetDataSize());
    resample.frame.timestamp = pkt.frame.timestamp;
    MessageChainImpl::onMsgProcessData(resample);
}

void AudioSource::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

void AudioSource::UpdateConfig(Message ptk) {
    m_config = json::parse(ptk.GetString());
    m_codecSampleRate = m_config["codecSampleRate"];
    m_codecChannelCount = m_config["codecChannelCount"];
    m_codecBytePerSample = m_config["codecBytePerSample"];
    m_codecSamplePerFrame = m_config["codecSamplePerFrame"];

    m_bufferSize = m_codecChannelCount * m_codecBytePerSample * m_codecSamplePerFrame;
    p_bufferPool = new BufferPool(2, m_bufferSize, 10);
    p_bufferPool->SetName("AudioSource");
}

