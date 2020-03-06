//
// Created by yongge on 20-2-21.
//

#ifndef ANDROID_VIDEOFILTER_H
#define ANDROID_VIDEOFILTER_H


#include <MediaChainImpl.h>
#include <MessageProcessor.h>
#include <MediaModule.h>

namespace freee {

    class VideoFilter : public MediaModule {

    public:
        VideoFilter(int mediaType = MediaType_Video,
                int mediaNumber = MediaNumber_VideoFilter,
                const std::string &mediaName = "VideoFilter");

        ~VideoFilter();

//        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;

    protected:
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessMedia(SmartPkt pkt) override;

        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(SmartPkt pkt) override;

    private:
        void FinalClear() override ;

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


#endif //ANDROID_VIDEOFILTER_H
