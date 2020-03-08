//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include <MessageContext.h>

namespace freee {

    class MediaContext : public MessageContext {

    public:

//        static MediaContext& Instance();
        static MediaContext* Instance();

        ~MediaContext() override ;

        SmartPkt GetStringPkt(int key, std::string str);

        void SendMessage(SmartPkt pkt) override;
        SmartPkt GetMessage(int key) override;

    public:

        void onRecvMessage(SmartPkt pkt) override;
        SmartPkt onObtainMessage(int key) override;

        MessageContext* ConnectCamera();
        MessageContext* ConnectMicrophone();

        void DisconnectCamera();
        void DisconnectMicrophone();

    private:

        MediaContext();
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
