//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include <MessageContext.h>

namespace freee {

    enum {
        MediaMessage_CreateRecord = 1,
        MediaMessage_CreateCamera,
        MediaMessage_GetRecordConfig,
    };

    class MediaContext : public MessageContext {

    public:

        static MediaContext* Instance();

        ~MediaContext() override ;

        void PutMessage(sr_message_t msg) override;

        sr_message_t GetMessage(sr_message_t msg) override;

    public:

        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    private:

        MediaContext(MessageContext *ctx);
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
