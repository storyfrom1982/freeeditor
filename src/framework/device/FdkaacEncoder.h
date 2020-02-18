//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_FDKAACENCODER_H
#define ANDROID_FDKAACENCODER_H


#include "AudioEncoder.h"

#include <fdk-aac/aacenc_lib.h>


namespace freee {


    class FdkaacEncoder : public AudioEncoder {
    public:
        int OpenAudioEncoder(json &cfg) override;

        void CloseAudioEncoder() override;

        int EncodeAudioData(SrMessage buffer) override;

    private:

        json mCfg;

        unsigned long m_intputSamples;
        unsigned long m_intputSize;
        unsigned long m_outputSize;

        HANDLE_AACENCODER m_pHandle;
    };


}


#endif //ANDROID_FDKAACENCODER_H
