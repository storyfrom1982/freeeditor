//
// Created by yongge on 20-3-5.
//

#include <android/MediaContext.h>
#include "MediaStream.h"
#include "FFmpegMediaStream.h"


using namespace freee;

enum {
    OnRecvMsg_ConnectStream = 11,
    OnRecvMsg_DisconnectStream = 12,
};

MediaStream::MediaStream(const std::string &mediaName, int mediaType)
        : MessageChain(mediaName, mediaType) {
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
    Message pkt = MediaContext::Instance()->GetStringPkt(OnRecvMsg_ConnectStream, url);
    ProcessMessage(pkt);
}

void MediaStream::DisconnectStream() {
    ProcessMessage(NewMessage(OnRecvMsg_DisconnectStream));
}
