//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_VIDEOSOURCE_H
#define ANDROID_VIDEOSOURCE_H


#include <MediaModule.h>
#include <MessageContext.h>

namespace freee {


    class VideoSource : public MessageChain {

    public:
        VideoSource(MessageContext *context = nullptr);
        ~VideoSource();

        void FinalClear() override;

        void Open(MessageChain *chain) override;

        void Close(MessageChain *chain) override;

        void Start(MessageChain *chain) override;

        void Stop(MessageChain *chain) override;

        void ProcessData(MessageChain *chain, Message pkt) override;


    private:
        void onRecvMessage(Message pkt) override;

        void UpdateMediaConfig(Message pkt);

    private:
        int m_status;
        int m_srcRotation;
        int m_srcWidth, m_srcHeight;
        int m_codecWidth, m_codecHeight;
        uint32_t m_srcImageFormat, m_codecImageFormat;

        size_t m_bufferSize;
        BufferPool *p_bufferPool = nullptr;
    };

}


#endif //ANDROID_VIDEOSOURCE_H
