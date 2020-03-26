//
// Created by yongge on 20-2-21.
//

#ifndef ANDROID_VIDEOFILTER_H
#define ANDROID_VIDEOFILTER_H


#include "../message/MessageProcessor.h"
#include "../message/MessageChain.h"


namespace freee {

    class VideoFilter : public MessageChain {

    public:
        VideoFilter(const std::string name = "VideoFilter");

        virtual ~VideoFilter();

//        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(Message pkt) override;

    private:
        void FinalClear() override ;

    private:
        int m_srcRotation;
        int m_srcWidth, m_srcHeight;
        int m_codecWidth, m_codecHeight;
        uint32_t m_srcImageFormat, m_codecImageFormat;

        size_t m_bufferSize;
        MessagePool *p_bufferPool = nullptr;

    };

}


#endif //ANDROID_VIDEOFILTER_H
