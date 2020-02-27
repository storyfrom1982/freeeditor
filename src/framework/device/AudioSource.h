//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOSOURCE_H
#define ANDROID_AUDIOSOURCE_H


#include <MessageContext.h>
#include <MediaChainImpl.h>


namespace freee {


    class AudioSource : public MessageContext, public MediaChainImpl {
    public:
        void FinalClear() override;

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

        virtual void onRecvMessage(SmartPkt msg) override;

        virtual SmartPkt onObtainMessage(int key) override;

    private:

    };


}



#endif //ANDROID_AUDIOSOURCE_H
