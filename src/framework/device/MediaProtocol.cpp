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
    StopMessageProcessor();
}

MediaProtocol::~MediaProtocol() {
    StopMessageProcessor();
}

void MediaProtocol::ProcessMessage(sr_message_t msg) {

    switch (msg.key){
        case ProtocolOpen:
            Connect(msg.str);
            break;
        case ProtocolClose:
            DisconnectContext();
            break;
        case ProtocolSendData:
            Write(msg);
            break;
        case ProtocolReadData:
            Read(msg);
            break;
        default:
            break;
    }
}

void MediaProtocol::OnPutMessage(sr_message_t msg) {
    MessageProcessor(msg);
}
