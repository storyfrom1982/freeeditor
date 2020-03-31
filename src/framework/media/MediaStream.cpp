//
// Created by yongge on 20-3-5.
//

#include <MediaContext.h>
#include "MediaStream.h"
#include "ffmpeg/FFmpegMediaStream.h"


using namespace freee;

enum {
    OnRecvMsg_ConnectStream = 11,
    OnRecvMsg_DisconnectStream = 12,
};

MediaStream::MediaStream(const std::string name) : MessageChain(name) {
    m_type = MediaType_Mix;
    StartProcessor();
}

MediaStream::~MediaStream() {
    StopProcessor();
    FinalClear();
}

void MediaStream::FinalClear() {

}

void MediaStream::onMsgControl(Message pkt) {
    switch (pkt.key()){
        case OnRecvMsg_ConnectStream:
            onMsgConnectStream(pkt);
            break;
        case OnRecvMsg_DisconnectStream:
            onMsgDisconnectStream();
            break;
        default:
            break;
    }
}

MediaStream *MediaStream::Create(std::string name) {
    return new FFmpegMediaStream();
}

void MediaStream::onMsgConnectStream(Message pkt) {

}

void MediaStream::onMsgDisconnectStream() {

}

void MediaStream::ConnectStream(std::string url) {
    Message msg = NewMessage(OnRecvMsg_ConnectStream, url);
    ProcessMessage(msg);
}

void MediaStream::DisconnectStream() {
    ProcessMessage(NewMessage(OnRecvMsg_DisconnectStream));
}
