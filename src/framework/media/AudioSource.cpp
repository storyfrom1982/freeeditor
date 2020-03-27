//
// Created by yongge on 20-2-15.
//

#include <MediaConfig.h>
#include <MediaContext.h>
#include "AudioSource.h"


using namespace freee;


AudioSource::AudioSource(std::string name) : MessageChain(name) {
//    MessageContext *context = MediaContext::Instance().ConnectMicrophone();
    m_type = MediaType_Audio;
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

void AudioSource::onRecvMessage(Message msg) {
    switch (msg.key()){
        case MsgKey_ProcessData:
            ProcessData(this, msg);
            break;
        case MsgKey_ProcessEvent:
            onRecvEvent(msg);
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
    SendMessage(NewMessage(MsgKey_Open, m_config.dump()));
}

void AudioSource::Close(MessageChain *chain) {
    SendMessage(NewMessage(MsgKey_Close));
}

void AudioSource::Start(MessageChain *chain) {
    SendMessage(NewMessage(MsgKey_Start));
}

void AudioSource::Stop(MessageChain *chain) {
    SendMessage(NewMessage(MsgKey_Stop));
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

void AudioSource::UpdateConfig(Message msg) {
    m_config = json::parse(msg.GetString());
    LOGD("AudioSource::UpdateConfig %s\n", msg.GetString().c_str());
    m_codecSampleRate = m_config[CFG_CODEC_SAMPLE_RATE];
    m_codecChannelCount = m_config[CFG_CODEC_CHANNEL_COUNT];
    m_codecBytePerSample = m_config[CFG_CODEC_BYTE_PER_SAMPLE];
    m_codecSamplePerFrame = m_config[CFG_CODEC_SAMPLE_PER_FRAME];

    m_bufferSize = m_codecChannelCount * m_codecBytePerSample * m_codecSamplePerFrame;
    p_bufferPool = new MessagePool("AudioSourceFramePool", m_bufferSize, 10, 64, 0, 0);
}

void AudioSource::onRecvEvent(Message msg)
{
    switch (msg.event()){
        case Status_Opened:
            m_status = Status_Opened;
            LOGD("AudioSource Opened\n");
            UpdateConfig(msg);
            msg.GetMessagePtr()->key = MsgKey_Open;
            MessageChain::onMsgOpen(msg);
//            ReportEvent(SmartPkt(Status_Opened + m_number));
            break;
        case Status_Closed:
            m_status = Status_Closed;
//            CloseNext();
            msg.GetMessagePtr()->key = MsgKey_Close;
            MessageChain::onMsgClose(msg);
//            ReportEvent(SmartPkt(Status_Closed + m_number));
            FinalClear();
            LOGD("AudioSource Closed\n");
            break;
        case Status_Started:
            m_status = Status_Started;
//            ReportEvent(SmartPkt(Status_Started + m_number));
            LOGD("AudioSource Started\n");
            break;
        case Status_Stopped:
//            ReportEvent(SmartPkt(Status_Stopped + m_number));
            m_status = Status_Stopped;
            LOGD("AudioSource Stopped\n");
            break;
        default:
            break;
    }
}

