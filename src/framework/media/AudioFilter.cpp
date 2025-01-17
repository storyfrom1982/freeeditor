//
// Created by yongge on 20-3-2.
//

#include "AudioFilter.h"

using namespace freee;

AudioFilter::AudioFilter(const std::string &name) : MessageChain(name) {
    m_type = MediaType_Audio;
    StartProcessor();
}

AudioFilter::~AudioFilter() {
    StopProcessor();
    FinalClear();
}

void AudioFilter::onMsgOpen(Message pkt) {
    m_config = static_cast<MessageChain *>(pkt.obj())->GetConfig(this);
    OpenMedia();
    MessageChain::onMsgOpen(pkt);
//    OpenNext();
}

void AudioFilter::onMsgClose(Message pkt) {
    CloseMedia();
    MessageChain::onMsgClose(pkt);
//    CloseNext();
}

void AudioFilter::onMsgProcessData(Message pkt) {
//    LOGD("AudioFilter::onMsgProcessMedia data %d\n", pkt.GetKey());
    ProcessMedia(pkt);
}

int AudioFilter::OpenMedia() {
    m_codecSampleRate = m_config[CFG_CODEC_SAMPLE_RATE];
    m_codecChannelCount = m_config[CFG_CODEC_CHANNEL_COUNT];
    m_codecBytePerSample = m_config[CFG_CODEC_BYTES_PER_SAMPLE];
    m_codecSamplePerFrame = m_config[CFG_CODEC_SAMPLES_PER_FRAME];

    m_bufferSize = m_codecChannelCount * m_codecBytePerSample * m_codecSamplePerFrame;
    p_bufferPool = new MessagePool(GetName() + "FramePool", m_bufferSize, 10, 64, 0, 0);

    return 0;
}

void AudioFilter::CloseMedia() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

int AudioFilter::ProcessMedia(Message pkt) {
    MessageChain::onMsgProcessData(pkt);
    return 0;
}

void AudioFilter::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}
