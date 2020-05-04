//
// Created by yongge on 20-2-15.
//

#include <MediaConfig.h>
#include <MediaContext.h>
#include "AudioSource.h"


using namespace freee;


AudioSource::AudioSource(std::string name) : MessageChain(name) {
    m_type = MediaType_Audio;
    MessageContext *context = MediaContext::Instance()->ConnectMicrophone();
    ConnectContext(context);
}

AudioSource::~AudioSource() {
    Close(this);
    DisconnectContext();
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

void AudioSource::ProcessData(MessageChain *chain, Message msg) {
    if (p_pipe){
        sr_pipe_write(p_pipe, (char*) msg.data(), msg.size());
        while (sr_pipe_readable(p_pipe) >= m_bufferSize){
            Message message = p_bufferPool->NewMessage(MsgKey_ProcessData, nullptr, m_bufferSize);
            sr_pipe_read(p_pipe, (char*) message.data(), message.size());
            message.msgFrame()->timestamp = 1000000L / m_codecSampleRate * m_totalSamples;
            m_totalSamples += m_codecSamplesPerFrame;
            MessageChain::onMsgProcessData(message);
        }
    }else {
        Message message = p_bufferPool->NewMessage(MsgKey_ProcessData,
                                                   msg.data(), msg.size());
        message.msgFrame()->timestamp = 1000000L / m_codecSampleRate * m_totalSamples;
        m_totalSamples += m_codecSamplesPerFrame;
        MessageChain::onMsgProcessData(message);
    }
}

void AudioSource::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
    if (p_pipe){
        sr_pipe_release(&p_pipe);
    }
}

void AudioSource::UpdateConfig(Message msg) {
    m_config = json::parse(msg.getString());
    m_codecSampleRate = m_config[CFG_CODEC_SAMPLE_RATE];
    m_codecChannelCount = m_config[CFG_CODEC_CHANNEL_COUNT];
    m_codecBytePerSample = m_config[CFG_CODEC_BYTES_PER_SAMPLE];
    m_codecSamplesPerFrame = m_config[CFG_CODEC_SAMPLES_PER_FRAME];
    m_srcSamplesPerFrame = m_config[CFG_SRC_SAMPLES_PER_FRAME];

    if (m_srcSamplesPerFrame != m_codecSamplesPerFrame){
        p_pipe = sr_pipe_create(1U << 18);
    }

    m_bufferSize = m_codecChannelCount * m_codecBytePerSample * m_codecSamplesPerFrame;
    p_bufferPool = new MessagePool(GetName() + "::FramePool", m_bufferSize, 10, 64, 0, 0);
}

void AudioSource::onRecvEvent(Message msg)
{
    switch (msg.i32()){
        case Status_Opened:
            m_status = Status_Opened;
            UpdateConfig(msg);
            MessageChain::onMsgOpen(NewMessage(MsgKey_Open));
            break;
        case Status_Closed:
            m_status = Status_Closed;
            MessageChain::onMsgClose(NewMessage(MsgKey_Close));
            FinalClear();
            break;
        case Status_Started:
            m_status = Status_Started;
            break;
        case Status_Stopped:
            m_status = Status_Stopped;
            break;
        default:
            break;
    }
}

