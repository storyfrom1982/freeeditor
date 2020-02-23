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

        void SendMessage(SmartMsg msg) override;
        SmartMsg GetMessage(int key) override;

    public:

        void onRecvMessage(SmartMsg msg) override;
        SmartMsg onObtainMessage(int key) override;

        MessageContext* ConnectCamera();
        MessageContext* ConnectMicrophone();

        void DisconnectCamera();
        void DisconnectMicrophone();

    private:

        MediaContext();
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
