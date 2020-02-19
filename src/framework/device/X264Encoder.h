//
// Created by yongge on 20-2-14.
//

#ifndef ANDROID_X264ENCODER_H
#define ANDROID_X264ENCODER_H


#include "VideoEncoder.h"

#include <x264/x264.h>
#include <x264/x264_config.h>


namespace freee {

    class X264Encoder : public VideoEncoder {

    public:
        X264Encoder(){}
        ~X264Encoder();

    protected:
        void OnEncodeVideo(SrPkt buffer) override;

    protected:
        int OnOpenEncoder(json& cfg) override;

        void OnCloseEncoder() override;

    private:

        ino64_t m_frameId;
        x264_t   *m_handle;
        x264_param_t param;
    };
}


#endif //ANDROID_X264ENCODER_H
