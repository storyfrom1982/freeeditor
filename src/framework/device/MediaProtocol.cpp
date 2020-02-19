//
// Created by yongge on 20-2-16.
//

#include "MediaProtocol.h"
#include "MediaFile.h"


using namespace freee;


enum {

    ProtocolOpen = 1,
    ProtocolClose = 2,
    ProtocolReadData = 4,
    ProtocolSendData = 5,

};


MediaProtocol *MediaProtocol::Create(std::string url) {
    return new MediaFile(url);
}

MediaProtocol::MediaProtocol(std::string url) {
    StopProcessor();
}

MediaProtocol::~MediaProtocol() {
    StopProcessor();
}

void MediaProtocol::onReceiveMessage(SrPkt msg) {
    MessageProcessor(msg);
}

void MediaProtocol::MessageProcessor(SrPkt pkt) {
    switch (pkt.msg.key){
        case ProtocolOpen:
            Connect(pkt.msg.js);
            break;
        case ProtocolClose:
            DisconnectContext();
            break;
        case ProtocolSendData:
            Write(pkt);
            break;
        case ProtocolReadData:
            Read(pkt);
            break;
        default:
            break;
    }
}
