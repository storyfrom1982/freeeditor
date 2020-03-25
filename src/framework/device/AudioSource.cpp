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


AudioSource::AudioSource(std::string mediaName, int mediaType)
        : MessageChain(mediaName, mediaType) {
//    MessageContext *context = MediaContext::Instance().ConnectMicrophone();
    MessageContext *context = MediaContext::Instance()->ConnectMicrophone();
    ConnectContext(context);
}

AudioSource::~AudioSource() {
    Close(this);
    DisconnectContext();
//    MediaContext::Instance().DisconnectMicrophone();
    MediaContext::Instance()->DisconnectMicrophone();
    FinalClear();
}

void AudioSource::onRecvMessage(Message pkt) {
    switch (pkt.key()){
        case OnRecvMsg_ProcessSound:
            ProcessData(this, pkt);
            break;
        case OnRecvMsg_Opened:
            m_status = Status_Opened;
            LOGD("AudioSource Opened\n");
            UpdateConfig(pkt);
            pkt.GetMessagePtr()->key = MsgKey_Open;
            MessageChain::onMsgOpen(pkt);
//            ReportEvent(SmartPkt(Status_Opened + m_number));
            break;
        case OnRecvMsg_Closed:
            m_status = Status_Closed;
//            CloseNext();
            pkt.GetMessagePtr()->key = MsgKey_Close;
            MessageChain::onMsgClose(pkt);
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

Message AudioSource::onRequestMessage(int key) {
    return MessageContext::onRequestMessage(key);
}

void AudioSource::Open(MessageChain *chain) {
    m_config = chain->GetConfig(this);
    SendMessage(NewMessage(SendMsg_Open, m_config.dump()));
}

void AudioSource::Close(MessageChain *chain) {
    SendMessage(NewMessage(SendMsg_Close));
}

void AudioSource::Start(MessageChain *chain) {
    SendMessage(NewMessage(SendMsg_Start));
}

void AudioSource::Stop(MessageChain *chain) {
    SendMessage(NewMessage(SendMsg_Stop));
}

void AudioSource::ProcessData(MessageChain *chain, Message pkt) {
    Message resample = p_bufferPool->NewMessage(MsgKey_ProcessData);
    memcpy(resample.GetBufferPtr(), pkt.GetFramePtr()->data, resample.GetBufferSize());
    resample.GetFramePtr()->timestamp = pkt.GetFramePtr()->timestamp;
    MessageChain::onMsgProcessData(resample);
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
    p_bufferPool = new MessagePool("AudioSourceFramePool", m_bufferSize, 10, 64, 0, 0);
}

