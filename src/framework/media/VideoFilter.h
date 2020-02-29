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

    public:
        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;

    protected:
        void MessageOpen(SmartPkt pkt) override;

        void MessageClose(SmartPkt pkt) override;

        void MessageProcessMedia(SmartPkt pkt) override;

        int ModuleOpen(json &cfg) override;

        void ModuleClose() override;

        int ModuleProcessMedia(SmartPkt pkt) override;

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
