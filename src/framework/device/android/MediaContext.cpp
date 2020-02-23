//
// Created by yongge on 20-2-2.
//

#include <MConfig.h>
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

SmartMsg MediaContext::onObtainMessage(int key) {
    if (key == OnGetMsg_CreateRecorder){
        return SmartMsg(key, new MediaRecorder());
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js = MConfig::load();
        return SmartMsg(key, js.dump());
    }
    return SmartMsg();
}

void MediaContext::onRecvMessage(SmartMsg msg) {
    if (msg.GetKey() == OnRecvMsg_DisconnectRecorder){
        MediaRecorder *recorder = static_cast<MediaRecorder *>(msg.GetPtr());
        delete recorder;
    }
}

void MediaContext::SendMessage(SmartMsg msg) {
    MessageContext::SendMessage(msg);
}

SmartMsg MediaContext::GetMessage(int key) {
    return MessageContext::GetMessage(key);
}

MessageContext *MediaContext::ConnectCamera() {
    SmartMsg msg = MessageContext::GetMessage(GetMsg_ConnectCamera);
    assert(msg.GetPtr());
    assert(msg.GetKey() == GetMsg_ConnectCamera);
    return static_cast<MessageContext *>(msg.GetPtr());
}

MessageContext *MediaContext::ConnectMicrophone() {
    SmartMsg msg = MessageContext::GetMessage(GetMsg_ConnectMicrophone);
    assert(msg.GetPtr());
    assert(msg.GetKey() == GetMsg_ConnectMicrophone);
    return static_cast<MessageContext *>(msg.GetPtr());
}

void MediaContext::DisconnectCamera() {
    SendMessage(SmartMsg(SendMsg_DisconnectCamera));
}

void MediaContext::DisconnectMicrophone() {
    SendMessage(SmartMsg(SendMsg_DisconnectMicrophone));
}
