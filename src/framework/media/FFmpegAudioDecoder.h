//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_FFMPEGAUDIODECODER_H
#define ANDROID_FFMPEGAUDIODECODER_H


#include "AudioDecoder.h"

namespace freee {


    class FFmpegAudioDecoder : public AudioDecoder
    {
    public:
        FFmpegAudioDecoder();
        ~FFmpegAudioDecoder();

    protected:
        int OpenDecoder() override;

        void CloseDecoder() override;

        int DecodeAudio(Message msg) override;
    };

}


#endif //ANDROID_FFMPEGAUDIODECODER_H
