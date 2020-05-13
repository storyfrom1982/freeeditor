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
        int OpenMedia(MediaPlugin *plugin) override;

        void CloseMedia(MediaPlugin *plugin) override;

        int ProcessMedia(MediaPlugin *plugin, Message msg) override;

    public:
        std::string &GetExtraConfig(MessageChain *chain) override;

    private:

        unsigned long m_inputSamples;
        unsigned long m_inputSize;
        unsigned long m_outputSize;

        HANDLE_AACENCODER m_pHandle = nullptr;
    };


}


#endif //ANDROID_FDKAACENCODER_H
