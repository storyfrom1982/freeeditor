//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include "MessageContext.h"

namespace freee {

    class MediaContext : public MessageContext {

    public:

//        static MediaContext& Instance();
        static MediaContext* Instance();

        ~MediaContext() override ;

        void SendMessage(Message pkt) override;
        Message RequestMessage(int key) override;

    public:

        void onRecvMessage(Message pkt) override;
        Message onRequestMessage(int key) override;

        MessageContext* ConnectCamera();
        MessageContext* ConnectMicrophone();
        MessageContext* ConnectSpeaker();

        void DisconnectCamera();
        void DisconnectMicrophone();
        void DisconnectSpeaker();

    private:

        MediaContext();
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
