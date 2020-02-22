//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include <MessageContext.h>

namespace freee {

    class MediaContext : public MessageContext {

    public:

        static MediaContext& Instance();

        ~MediaContext() override ;

        void SendMessage(MediaPacket msg) override;
        MediaPacket GetMessage(int key) override;

    public:

        void onRecvMessage(MediaPacket msg) override;
        MediaPacket onObtainMessage(int key) override;

        MessageContext* ConnectCamera();
        MessageContext* ConnectMicrophone();

    private:

        MediaContext();
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
