//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include <MessageContext.h>

namespace freee {

    class MediaContext : public MessageContext {

    public:

        static MediaContext* Instance();

        ~MediaContext() override ;

        void SendMessage(SrPkt msg) override;
        SrPkt GetMessage(int key) override;

    public:

        void onReceiveMessage(SrPkt msg) override;
        SrPkt onObtainMessage(int key) override;

        MessageContext* CreateCamera();
        MessageContext* CreateMicrophone();

    private:

        MediaContext(MessageContext *ctx);
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
