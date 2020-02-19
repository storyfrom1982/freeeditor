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
    GetMsg_CreateCamera = 1,
    GetMsg_CreateMicrophone = 2,
};

MediaContext::MediaContext(){
//    ConnectContext(ctx);
}

MediaContext::~MediaContext() = default;

//static MediaContext *globalMediaContext = nullptr;

MediaContext& MediaContext::Instance() {
    static MediaContext globalMediaContext;
//    if (!globalMediaContext){
//        globalMediaContext = new MediaContext();
//    }
    return globalMediaContext;
}

SrPkt MediaContext::onObtainMessage(int key) {
    SrPkt pkt;
    if (key == OnGetMsg_CreateRecorder){
        pkt.msg.key = key;
        pkt.msg.ptr = new MediaRecorder();
    }else if (key == OnGetMsg_GetRecorderConfig){
        json js;
        MConfig::load(js, "");
        std::string str = js.dump();
        pkt.msg.key = key;
        pkt.msg.size = str.length();
        pkt.msg.js = strndup(str.c_str(), pkt.msg.size);
    }
    return pkt;
}

void MediaContext::onReceiveMessage(SrPkt msg) {

}

void MediaContext::SendMessage(SrPkt msg) {
    MessageContext::SendMessage(msg);
}

SrPkt MediaContext::GetMessage(int key) {
    return MessageContext::GetMessage(key);
}

MessageContext *MediaContext::CreateCamera() {
    SrPkt pkt = MessageContext::GetMessage(GetMsg_CreateCamera);
    if (pkt.msg.key != GetMsg_CreateCamera || pkt.msg.ptr == NULL){
        return nullptr;
    }
    return static_cast<MessageContext *>(pkt.msg.ptr);
}

MessageContext *MediaContext::CreateMicrophone() {
    SrPkt pkt = MessageContext::GetMessage(GetMsg_CreateMicrophone);
    if (pkt.msg.key != GetMsg_CreateMicrophone || pkt.msg.ptr == NULL){
        return nullptr;
    }
    return static_cast<MessageContext *>(pkt.msg.ptr);
}
