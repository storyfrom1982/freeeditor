//
// Created by yongge on 20-2-2.
//

#include <MConfig.h>
#include <BufferPool.h>
#include "MediaContext.h"
#include "MediaRecorder.h"

using namespace freee;

enum {
    OnGetMsg_CreateRecorder = 1,
    OnGetMsg_GetRecorderConfig = 2,
};

enum {
    GetMsg_ConnectCamera = 1,
    GetMsg_ConnectMicrophone = 2,
};

enum {
    OnRecvMsg_DisconnectRecorder = 1,
};

enum {
    SendMsg_DisconnectCamera = 1,
    SendMsg_DisconnectMicrophone = 2,
};

static MediaContext *sMediaContext = nullptr;

MediaContext::MediaContext() : MessageContext("MediaContext"){};

MediaContext::~MediaContext(){
    sMediaContext = nullptr;
};

MediaContext* MediaContext::Instance() {
    if (sMediaContext == nullptr){
        sMediaContext = new MediaContext();
    }
    return sMediaContext;
}

//MediaContext& MediaContext::Instance() {
//    static MediaContext globalMediaContext;
//    return globalMediaContext;
//}

Message MediaContext::onObtainMessage(int key) {
    if (key == OnGetMsg_CreateRecorder){
        return Message(key, new MediaRecorder());
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js = MConfig::load();
        return NewJsonPkt(key, js.dump());
    }
    return Message();
}

void MediaContext::onRecvMessage(Message pkt) {
    if (pkt.GetKey() == OnRecvMsg_DisconnectRecorder){
        MediaRecorder *recorder = static_cast<MediaRecorder *>(pkt.GetPtr());
        delete recorder;
    }
}

void MediaContext::SendMessage(Message pkt) {
    MessageContext::SendMessage(pkt);
}

Message MediaContext::RequestMessage(int key) {
    return MessageContext::RequestMessage(key);
}

MessageContext *MediaContext::ConnectCamera() {
    Message pkt = MessageContext::RequestMessage(GetMsg_ConnectCamera);
    assert(pkt.GetPtr());
    assert(pkt.GetKey() == GetMsg_ConnectCamera);
    return static_cast<MessageContext *>(pkt.GetPtr());
}

MessageContext *MediaContext::ConnectMicrophone() {
    Message pkt = MessageContext::RequestMessage(GetMsg_ConnectMicrophone);
    assert(pkt.GetPtr());
    assert(pkt.GetKey() == GetMsg_ConnectMicrophone);
    return static_cast<MessageContext *>(pkt.GetPtr());
}

void MediaContext::DisconnectCamera() {
    SendMessage(Message(SendMsg_DisconnectCamera));
}

void MediaContext::DisconnectMicrophone() {
    SendMessage(Message(SendMsg_DisconnectMicrophone));
}

Message MediaContext::GetStringPkt(int key, std::string str) {
    return NewJsonPkt(key, str);
}
