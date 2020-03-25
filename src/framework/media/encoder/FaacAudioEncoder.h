//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_FDKAACENCODER_H
#define ANDROID_FDKAACENCODER_H



#include <fdk-aac/aacenc_lib.h>
#include "../AudioEncoder.h"


namespace freee {


    class FaacAudioEncoder : public AudioEncoder {
    public:
        FaacAudioEncoder();
        ~FaacAudioEncoder();

    protected:
        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(Message pkt) override;

    public:
        std::string &GetExtraConfig(MessageChain *chain) override;

    private:

        unsigned long m_intputSamples;
        unsigned long m_intputSize;
        unsigned long m_outputSize;

        HANDLE_AACENCODER m_pHandle = nullptr;
    };


}


#endif //ANDROID_FDKAACENCODER_H
