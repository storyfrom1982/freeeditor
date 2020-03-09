//
// Created by yongge on 20-3-2.
//

#include "AudioFilter.h"

using namespace freee;

AudioFilter::AudioFilter(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    StartProcessor("AudioFilter");
}

AudioFilter::~AudioFilter() {
    StopProcessor();
    FinalClear();
}

void AudioFilter::onMsgOpen(Message pkt) {
    m_config = static_cast<MessageChain *>(pkt.GetPtr())->GetConfig(this);
    OpenModule();
    MessageChain::onMsgOpen(pkt);
//    OpenNext();
}

void AudioFilter::onMsgClose(Message pkt) {
    CloseModule();
    MessageChain::onMsgClose(pkt);
//    CloseNext();
}

void AudioFilter::onMsgProcessData(Message pkt) {
//    LOGD("AudioFilter::onMsgProcessMedia data %d\n", pkt.GetKey());
    ProcessMediaByModule(pkt);
}

int AudioFilter::OpenModule() {
    m_codecSampleRate = m_config["codecSampleRate"];
    m_codecChannelCount = m_config["codecChannelCount"];
    m_codecBytePerSample = m_config["codecBytePerSample"];
    m_codecSamplePerFrame = m_config["codecSamplePerFrame"];

    m_bufferSize = m_codecChannelCount * m_codecBytePerSample * m_codecSamplePerFrame;
    p_bufferPool = new BufferPool(2, m_bufferSize, 10);
    p_bufferPool->SetName(m_name);

    return 0;
}

void AudioFilter::CloseModule() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

int AudioFilter::ProcessMediaByModule(Message pkt) {
    MessageChain::onMsgProcessData(pkt);
    return 0;
}

void AudioFilter::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}
