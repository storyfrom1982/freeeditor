//
// Created by yongge on 20-3-3.
//

#include "AudioEncoder.h"
#include "FaacAudioEncoder.h"

freee::AudioEncoder::AudioEncoder(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    StartProcessor(mediaName);
}

freee::AudioEncoder::~AudioEncoder() {
    StopProcessor();
    FinalClear();
}

void freee::AudioEncoder::onMsgOpen(freee::SmartPkt pkt) {
    MediaChain *chain = static_cast<MediaChain *>(pkt.GetPtr());
    m_config = chain->GetConfig(this);
    size_t bytePerSample = m_config["codecBytePerSample"];
    size_t samplePerFrame = m_config["codecSamplePerFrame"];
    m_bufferSize = bytePerSample * samplePerFrame;
    p_bufferPool = new BufferPool(2, m_bufferSize, 10, 16);
    OpenModule();
    pkt.frame.media_type = MediaType_Audio;
    MediaChainImpl::onMsgOpen(pkt);
}

void freee::AudioEncoder::onMsgClose(freee::SmartPkt pkt) {
    CloseModule();
    MediaChainImpl::onMsgClose(pkt);
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

void freee::AudioEncoder::onMsgProcessMedia(freee::SmartPkt pkt) {
    ProcessMediaByModule(pkt);
}

void freee::AudioEncoder::onMsgControl(freee::SmartPkt pkt) {

}

freee::AudioEncoder *freee::AudioEncoder::Create(std::string name) {
    return new FaacAudioEncoder();
}

void freee::AudioEncoder::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}
