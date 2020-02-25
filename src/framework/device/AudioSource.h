//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOSOURCE_H
#define ANDROID_AUDIOSOURCE_H


#include <MessageContext.h>
#include <MediaChainImpl.h>
#include "AudioEncoder.h"

namespace freee {


    class AudioSource : public MessageContext, public MediaChainImpl {

    public:

        AudioSource(int mediaType = MediaType_Audio,
                int mediaNumber = MediaNumber_AudioSource,
                std::string mediaName = "AudioSource");

        ~AudioSource();

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;

        virtual void onRecvMessage(SmartMsg msg) override;

        virtual SmartMsg onObtainMessage(int key) override;

    private:
        AudioEncoder *mEncoder;
    };


}



#endif //ANDROID_AUDIOSOURCE_H
