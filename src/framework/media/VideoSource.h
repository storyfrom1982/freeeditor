//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_VIDEOSOURCE_H
#define ANDROID_VIDEOSOURCE_H


#include <MediaModule.h>
#include <MessageContext.h>

namespace freee {


    class VideoSource : public MessageContext, public MediaChainImpl {

    public:
        VideoSource(MessageContext *context = nullptr);
        ~VideoSource();

        void FinalClear() override;

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;


    private:
        void onRecvMessage(SmartPkt pkt) override;

        void UpdateMediaConfig(SmartPkt pkt);

    private:
        int m_status;
        int m_srcWidth, m_srcHeight;
        uint32_t m_srcImageFormat;
    };

}


#endif //ANDROID_VIDEOSOURCE_H
