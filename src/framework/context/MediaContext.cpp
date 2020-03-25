//
// Created by yongge on 20-2-2.
//

#include <MediaConfig.h>
#include "MessagePool.h"
#include "MediaContext.h"
#include "MediaRecorder.h"
#include "MediaPlayer.h"

using namespace freee;

enum {
    SendMsg_DisconnectCamera = 1,
    SendMsg_DisconnectMicrophone = 2,
    SendMsg_DisconnectSpeaker = 3,
};

enum {
    ReqMsg_ConnectCamera = 1,
    ReqMsg_ConnectMicrophone = 2,
    ReqMsg_ConnectSpeaker = 3,
};

enum {
    OnReqMsg_CreateRecorder = 1,
    OnReqMsg_CreatePlayer = 2,
    OnReqMsg_GetRecorderConfig = 3,
};

enum {
    OnRecvMsg_DisconnectRecorder = 1,
    OnRecvMsg_DisconnectPlayer = 2,
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

Message MediaContext::onRequestMessage(int key) {
    if (key == OnReqMsg_CreateRecorder){
        return NewMessage(key, new MediaRecorder());
    }else if (key == OnReqMsg_CreatePlayer){
        return NewMessage(key, new MediaPlayer());
    }else if (key == OnReqMsg_GetRecorderConfig){
        json js = MediaConfig::load();
        return NewMessage(key, js.dump());
    }
    return NewMessage(0);
}

void MediaContext::onRecvMessage(Message pkt) {
    if (pkt.key() == OnRecvMsg_DisconnectRecorder){
        MediaRecorder *pRecorder = static_cast<MediaRecorder *>(pkt.GetObjectPtr());
        delete pRecorder;
    }else if (pkt.key() == OnRecvMsg_DisconnectPlayer){
        MediaPlayer *pPlayer = static_cast<MediaPlayer *>(pkt.GetObjectPtr());
        delete pPlayer;
    }
}

void MediaContext::SendMessage(Message pkt) {
    MessageContext::SendMessage(pkt);
}

Message MediaContext::RequestMessage(int key) {
    return MessageContext::RequestMessage(key);
}

MessageContext *MediaContext::ConnectCamera() {
    Message pkt = MessageContext::RequestMessage(ReqMsg_ConnectCamera);
    assert(pkt.GetObjectPtr());
    assert(pkt.key() == ReqMsg_ConnectCamera);
    return static_cast<MessageContext *>(pkt.GetObjectPtr());
}

MessageContext *MediaContext::ConnectMicrophone() {
    Message pkt = MessageContext::RequestMessage(ReqMsg_ConnectMicrophone);
    assert(pkt.GetObjectPtr());
    assert(pkt.key() == ReqMsg_ConnectMicrophone);
    return static_cast<MessageContext *>(pkt.GetObjectPtr());
}

MessageContext *MediaContext::ConnectSpeaker()
{
    Message pkt = MessageContext::RequestMessage(ReqMsg_ConnectSpeaker);
    assert(pkt.GetObjectPtr());
    assert(pkt.key() == ReqMsg_ConnectSpeaker);
    return static_cast<MessageContext *>(pkt.GetObjectPtr());
}

void MediaContext::DisconnectCamera() {
    SendMessage(NewMessage(SendMsg_DisconnectCamera));
}

void MediaContext::DisconnectMicrophone() {
    SendMessage(NewMessage(SendMsg_DisconnectMicrophone));
}

void MediaContext::DisconnectSpeaker()
{
    SendMessage(NewMessage(SendMsg_DisconnectSpeaker));
}

Message MediaContext::GetStringPkt(int key, std::string str) {
    return NewMessage(key, str);
}
