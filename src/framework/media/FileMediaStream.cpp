//
// Created by yongge on 20-3-6.
//

#include "FileMediaStream.h"


using namespace freee;

FileMediaStream::FileMediaStream() {
    m_pContext = nullptr;
    m_streamCount = 0;
    m_Streams = std::vector<AVStream*>(3);
}

FileMediaStream::~FileMediaStream() {

}

void FileMediaStream::ConnectStream(std::string url) {
    if (avformat_alloc_output_context2(&m_pContext, NULL, NULL, url.c_str()) < 0) {
        LOGD("[FileMediaStream] ConnectStream failed to %s\n", url.c_str());
        return;
    }
}

void FileMediaStream::DisconnectStream() {
    MediaStream::DisconnectStream();
}

int FileMediaStream::OpenModule() {
    return 0;
}

void FileMediaStream::CloseModule() {

}

int FileMediaStream::ProcessMediaByModule(SmartPkt pkt) {
    return MediaModule::ProcessMediaByModule(pkt);
}
