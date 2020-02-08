//
// Created by yongge on 20-2-2.
//

#include <MConfig.h>
#include "MediaContext.h"
#include "MediaRecord.h"

using namespace freee;

MediaContext::MediaContext(MessageContext *ctx){
    SetContextHandler(ctx);
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
    if (msg.key == MediaMessage_CreateRecord){
        msg.ptr = new MediaRecord();
        msg.size = MessageType_Pointer;
    }else if (msg.key == MediaMessage_GetRecordConfig){
        json js;
        MConfig::load(js, "");
        std::string str = js.dump();
        msg.size = str.length();
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
