//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_VIDEODECODER_H
#define ANDROID_VIDEODECODER_H


#include <MessageChain.h>


namespace freee {

    class VideoDecoder : public MessageChain
    {
    public:
        static VideoDecoder* Create(std::string name);
        VideoDecoder(const std::string &name = "VideoDecoder", int type = MediaType_Video);
        ~VideoDecoder();

    protected:
        void onMsgOpen(Message msg) override;

        void onMsgClose(Message msg) override;

        void onMsgProcessData(Message msg) override;

        void onMsgProcessEvent(Message msg) override;

        void onMsgControl(Message msg) override;

    protected:
        virtual int OpenDecoder() = 0;
        virtual void CloseDecoder() = 0;
        virtual int DecodeVideo(Message msg) = 0;

    };
}


#endif //ANDROID_VIDEODECODER_H
