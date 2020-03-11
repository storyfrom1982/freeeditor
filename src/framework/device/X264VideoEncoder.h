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

        std::string &GetExtraConfig(MessageChain *chain) override;

        virtual int OpenEncoder() override;

        virtual void CloseEncoder() override;

        virtual int EncoderEncode(Message pkt) override;

    private:
        std::string GenAvc1();
        std::string GenH264Config();

    private:
        bool is_opened = false;
        x264_t   *m_handle = nullptr;
        x264_param_t m_param;
    };
}


#endif //ANDROID_X264ENCODER_H
