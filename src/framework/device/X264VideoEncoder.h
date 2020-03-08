//
// Created by yongge on 20-2-14.
//

#ifndef ANDROID_X264ENCODER_H
#define ANDROID_X264ENCODER_H


#include "VideoEncoder.h"

#include <x264/x264.h>
#include <x264/x264_config.h>


namespace freee {

    class X264VideoEncoder : public VideoEncoder {

    public:
        X264VideoEncoder();
        ~X264VideoEncoder();

        std::string GetExtraConfig(MediaChain *chain) override;

    protected:
        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(SmartPkt pkt) override;

    private:
        std::string GenAvc1();
        std::string GenH264Config();

    private:
        std::string m_extraConfig;
        x264_t   *m_handle;
        x264_param_t m_param;
    };
}


#endif //ANDROID_X264ENCODER_H
