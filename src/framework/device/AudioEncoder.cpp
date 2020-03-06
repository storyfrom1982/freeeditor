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
}

void freee::AudioEncoder::onMsgOpen(freee::SmartPkt pkt) {
    MediaChain *chain = static_cast<MediaChain *>(pkt.GetPtr());
    m_config = chain->GetConfig(this);
    OpenModule();
}

void freee::AudioEncoder::onMsgClose(freee::SmartPkt pkt) {
    CloseModule();
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

}
