//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOSOURCE_H
#define ANDROID_AUDIOSOURCE_H


#include <MessageContext.h>
#include "AudioEncoder.h"

namespace freee {


    class AudioSource : public MessageContext {

    public:

        AudioSource();

        ~AudioSource();

        void Open(json& cfg);

        void Close();

        void Start();

        void Stop();

        void SetEncoder(AudioEncoder *encoder);

        virtual void onRecvMessage(SmartMsg msg);

        virtual SmartMsg onObtainMessage(int key);

    private:
        AudioEncoder *mEncoder;
    };


}



#endif //ANDROID_AUDIOSOURCE_H
