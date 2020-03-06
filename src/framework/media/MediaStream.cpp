//
// Created by yongge on 20-3-5.
//

#include "MediaStream.h"


using namespace freee;

MediaStream::MediaStream(int mediaType, int mediaNumber, const std::string &mediaName)
        : MediaModule(mediaType, mediaNumber, mediaName) {
    StartProcessor(mediaName);
}

MediaStream::~MediaStream() {
    StopProcessor();
    FinalClear();
}

void MediaStream::FinalClear() {

}

void MediaStream::onMsgOpen(SmartPkt pkt) {
    m_configList.push_back(static_cast<MediaChain *>(pkt.GetPtr())->GetConfig(this));
    OpenModule();
}

void MediaStream::onMsgClose(SmartPkt pkt) {
    CloseModule();
}

void MediaStream::onMsgProcessMedia(SmartPkt pkt) {
    ProcessMediaByModule(pkt);
}

void MediaStream::onMsgControl(SmartPkt pkt) {
    MediaChainImpl::onMsgControl(pkt);
}

MediaStream *MediaStream::Create(std::string name) {
    return nullptr;
}
