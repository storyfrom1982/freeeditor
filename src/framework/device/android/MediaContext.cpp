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

MediaContext::MediaContext(){
    SetContextName("MediaContext");
};

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

SmartPkt MediaContext::onObtainMessage(int key) {
    if (key == OnGetMsg_CreateRecorder){
        return SmartPkt(key, new MediaRecorder());
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js = MConfig::load();
        return NewJsonPkt(key, js.dump());
    }
    return SmartPkt();
}

void MediaContext::onRecvMessage(SmartPkt pkt) {
    if (pkt.GetKey() == OnRecvMsg_DisconnectRecorder){
        MediaRecorder *recorder = static_cast<MediaRecorder *>(pkt.GetPtr());
        delete recorder;
    }
}

void MediaContext::SendMessage(SmartPkt pkt) {
    MessageContext::SendMessage(pkt);
}

SmartPkt MediaContext::GetMessage(int key) {
    return MessageContext::GetMessage(key);
}

MessageContext *MediaContext::ConnectCamera() {
    SmartPkt pkt = MessageContext::GetMessage(GetMsg_ConnectCamera);
    assert(pkt.GetPtr());
    assert(pkt.GetKey() == GetMsg_ConnectCamera);
    return static_cast<MessageContext *>(pkt.GetPtr());
}

MessageContext *MediaContext::ConnectMicrophone() {
    SmartPkt pkt = MessageContext::GetMessage(GetMsg_ConnectMicrophone);
    assert(pkt.GetPtr());
    assert(pkt.GetKey() == GetMsg_ConnectMicrophone);
    return static_cast<MessageContext *>(pkt.GetPtr());
}

void MediaContext::DisconnectCamera() {
    SendMessage(SmartPkt(SendMsg_DisconnectCamera));
}

void MediaContext::DisconnectMicrophone() {
    SendMessage(SmartPkt(SendMsg_DisconnectMicrophone));
}
