//
// Created by yongge on 20-2-2.
//

#include <MConfig.h>
#include "MediaContext.h"
#include "MediaRecord.h"

using namespace freee;

enum {
    OnGetMsg_CreateRecorder = 1,
    OnGetMsg_GetRecorderConfig = 2,
};

enum {
    GetMsg_CreateCamera = 1,
    GetMsg_CreateMicrophone = 2,
};

MediaContext::MediaContext(MessageContext *ctx){
    ConnectContextHandler(ctx);
}

MediaContext::~MediaContext() = default;

static MediaContext *globalMediaContext = nullptr;

MediaContext *MediaContext::Instance() {
    if (!globalMediaContext){
        globalMediaContext = new MediaContext(nullptr);
    }
    return globalMediaContext;
}

sr_message_t MediaContext::OnGetMessage(sr_message_t msg) {
    if (msg.key == OnGetMsg_CreateRecorder){
        msg.ptr = new MediaRecord();
        msg.type = MessageType_Pointer;
    }else if (msg.key == OnGetMsg_GetRecorderConfig){
        json js;
        MConfig::load(js, "");
        std::string str = js.dump();
        msg.type = str.length();
        msg.ptr = strdup(str.c_str());
    }else {
        msg = __sr_null_msg;
    }
    return msg;
}

void MediaContext::OnPutMessage(sr_message_t msg) {

}

void MediaContext::PutMessage(sr_message_t msg) {
    MessageContext::PutMessage(msg);
}

sr_message_t MediaContext::GetMessage(sr_message_t msg) {
    return MessageContext::GetMessage(msg);
}

MessageContext *MediaContext::CreateCamera() {
    sr_message_t msg = __sr_null_msg;
    msg.key = GetMsg_CreateCamera;
    msg.type = MessageType_Pointer;
    msg = MessageContext::GetMessage(msg);
    if (msg.key != GetMsg_CreateCamera){
        return nullptr;
    }
    return static_cast<MessageContext *>(msg.ptr);
}

MessageContext *MediaContext::CreateMicrophone() {
    sr_message_t msg = __sr_null_msg;
    msg.key = GetMsg_CreateMicrophone;
    msg.type = MessageType_Pointer;
    msg = MessageContext::GetMessage(msg);
    if (msg.key != GetMsg_CreateMicrophone){
        return nullptr;
    }
    return static_cast<MessageContext *>(msg.ptr);
}
