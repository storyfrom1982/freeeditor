//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_FFMPEGVIDEODECODER_H
#define ANDROID_FFMPEGVIDEODECODER_H


#include "VideoDecoder.h"

namespace freee {


    class FFmpegVideoDecoder : public VideoDecoder
    {
    public:
        FFmpegVideoDecoder();
        ~FFmpegVideoDecoder();

    protected:
        int OpenDecoder() override;

        void CloseDecoder() override;

        int DecodeVideo(Message msg) override;
    };

}


#endif //ANDROID_FFMPEGVIDEODECODER_H
