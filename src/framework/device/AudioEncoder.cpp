//
// Created by yongge on 20-3-3.
//

#include "AudioEncoder.h"
#include "FaacAudioEncoder.h"

freee::AudioEncoder::AudioEncoder(const std::string &mediaName, int mediaType)
        : MessageChain(mediaName, mediaType) {
    StartProcessor();
}

freee::AudioEncoder::~AudioEncoder() {
    StopProcessor();
    FinalClear();
}

void freee::AudioEncoder::onMsgOpen(freee::Message pkt) {
    MessageChain *chain = static_cast<MessageChain *>(pkt.GetPtr());
    m_config = chain->GetConfig(this);
    size_t bytePerSample = m_config["codecBytePerSample"];
    size_t samplePerFrame = m_config["codecSamplePerFrame"];
    m_bufferSize = bytePerSample * samplePerFrame;
    p_bufferPool = new MessagePool(m_bufferSize, 1, 64, 16, 0, "AudioEncoder");
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
    if (m_outputChainStatus == Status_Opened){
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
    switch (pkt.GetSubKey()){
        case MsgKey_Open:
            m_outputChainStatus = Status_Opened;
            break;
        case MsgKey_Close:
            m_outputChainStatus = Status_Closed;
            break;
        default:
            break;
    }
}
