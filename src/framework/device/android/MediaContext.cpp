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


MediaContext::MediaContext() = default;

MediaContext::~MediaContext() = default;


MediaContext& MediaContext::Instance() {
    static MediaContext globalMediaContext;
    return globalMediaContext;
}

SmartPkt MediaContext::onObtainMessage(int key) {
    SmartPkt pkt(key);
    if (key == OnGetMsg_CreateRecorder){
        pkt.msg.ptr = new MediaRecorder();
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js = MConfig::load();
        std::string str = js.dump();
        pkt.msg.size = str.length();
        pkt.msg.json = strndup(str.c_str(), pkt.msg.size);
    }else{
        return SmartPkt();
    }
    return pkt;
}

void MediaContext::onRecvMessage(SmartPkt msg) {

}

void MediaContext::SendMessage(SmartPkt msg) {
    MessageContext::SendMessage(msg);
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
