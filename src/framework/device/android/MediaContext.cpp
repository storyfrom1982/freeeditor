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


MediaContext::MediaContext() = default;

MediaContext::~MediaContext() = default;


MediaContext& MediaContext::Instance() {
    static MediaContext globalMediaContext;
    return globalMediaContext;
}

SmartPkt MediaContext::onObtainMessage(int key) {
    if (key == OnGetMsg_CreateRecorder){
        return SmartPkt(key, new MediaRecorder());
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js = MConfig::load();
        std::string str = js.dump();
        return SmartPkt(key, str.c_str(), str.length());
    }
    return SmartPkt();
}

void MediaContext::onRecvMessage(SmartPkt pkt) {
    if (pkt.msg.key == OnRecvMsg_DisconnectRecorder){
        MediaRecorder *recorder = static_cast<MediaRecorder *>(pkt.msg.ptr);
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
    assert(pkt.msg.ptr);
    assert(pkt.msg.key == GetMsg_ConnectCamera);
    return static_cast<MessageContext *>(pkt.msg.ptr);
}

MessageContext *MediaContext::ConnectMicrophone() {
    SmartPkt pkt = MessageContext::GetMessage(GetMsg_ConnectMicrophone);
    assert(pkt.msg.ptr);
    assert(pkt.msg.key == GetMsg_ConnectMicrophone);
    return static_cast<MessageContext *>(pkt.msg.ptr);
}

void MediaContext::DisconnectCamera() {
    SendMessage(SmartPkt(SendMsg_DisconnectCamera));
}

void MediaContext::DisconnectMicrophone() {
    SendMessage(SmartPkt(SendMsg_DisconnectMicrophone));
}
