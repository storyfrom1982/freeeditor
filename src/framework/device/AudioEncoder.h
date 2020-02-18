//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOENCODER_H
#define ANDROID_AUDIOENCODER_H


#include <MConfig.h>
#include <sr_buffer_pool.h>
#include <SrBufferPool.h>
#include "MediaProtocol.h"

namespace freee {


    class AudioEncoder {


    public:

        static AudioEncoder* Create(std::string name);

        virtual int OpenAudioEncoder(json& cfg) = 0;
        virtual void CloseAudioEncoder() = 0;
        virtual int EncodeAudioData(SrMessage buffer) = 0;

        void SetProtocol(MediaProtocol *aProtocol){
            mediaProtocol = aProtocol;
        }

    protected:

        MediaProtocol *mediaProtocol;

    };


}


#endif //ANDROID_AUDIOENCODER_H
