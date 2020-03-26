//
// Created by yongge on 20-3-3.
//

#include "AudioEncoder.h"
#include "encoder/FaacAudioEncoder.h"

freee::AudioEncoder::AudioEncoder(const std::string name) : MessageChain(name) {
    m_type = MediaType_Audio;
    StartProcessor();
}

freee::AudioEncoder::~AudioEncoder() {
    StopProcessor();
    FinalClear();
}

void freee::AudioEncoder::onMsgOpen(freee::Message pkt) {
    MessageChain *chain = static_cast<MessageChain *>(pkt.GetObjectPtr());
    m_config = chain->GetConfig(this);
    size_t bytePerSample = m_config[CFG_CODEC_BYTE_PER_SAMPLE];
    size_t samplePerFrame = m_config[CFG_CODEC_SAMPLE_PER_FRAME];
    m_bufferSize = bytePerSample * samplePerFrame;
    p_bufferPool = new MessagePool("AudioEncoderFramePool", m_bufferSize, 1, 64, 16, 0);
    OpenModule();
    pkt.GetFramePtr()->type = MediaType_Audio;
    MessageChain::onMsgOpen(pkt);
}

void freee::AudioEncoder::onMsgClose(freee::Message pkt) {
    CloseModule();
    MessageChain::onMsgClose(pkt);
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

void freee::AudioEncoder::onMsgProcessData(freee::Message pkt) {
    if (m_chainStatus == Status_Opened){
        if (m_startTimestamp == -1){
            m_startTimestamp = pkt.GetFramePtr()->timestamp;
        }
        pkt.GetFramePtr()->timestamp -= m_startTimestamp;
        ProcessMediaByModule(pkt);
    }
}

void freee::AudioEncoder::onMsgControl(freee::Message pkt) {

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

void freee::AudioEncoder::onMsgProcessEvent(freee::Message pkt) {
    switch (pkt.event()){
        case MsgKey_Open:
            m_chainStatus = Status_Opened;
            break;
        case MsgKey_Close:
            m_chainStatus = Status_Closed;
            break;
        default:
            break;
    }
}
