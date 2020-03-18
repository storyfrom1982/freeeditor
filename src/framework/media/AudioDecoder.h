//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_AUDIODECODER_H
#define ANDROID_AUDIODECODER_H


#include <MessageChain.h>

namespace freee {

    class AudioDecoder : public MessageChain
    {
    public:
        static AudioDecoder* Create(std::string name);
        AudioDecoder(const std::string &name = "AudioDecoder", int type = MediaType_Audio);
        ~AudioDecoder();

    protected:
        void onMsgOpen(Message msg) override;

        void onMsgClose(Message msg) override;

        void onMsgProcessData(Message msg) override;

        void onMsgControl(Message msg) override;

    protected:
        virtual int OpenDecoder() = 0;
        virtual void CloseDecoder() = 0;
        virtual int DecodeAudio(Message msg) = 0;
    };

}


#endif //ANDROID_AUDIODECODER_H
