//
// Created by yongge on 20-2-21.
//

#ifndef ANDROID_VIDEOFILTER_H
#define ANDROID_VIDEOFILTER_H


#include <MediaChainImpl.h>
#include <MediaProcessor.h>
#include <MediaModule.h>

namespace freee {

    class VideoFilter : public MediaModule, MediaProcessor {

    public:
        VideoFilter(int mediaType = MediaType_Video,
                int mediaNumber = MediaNumber_VideoFilter,
                const std::string &mediaName = "VideoFilter");

        ~VideoFilter();

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, MediaPacket pkt) override;

    protected:
        void ProcessOpen() override;

        void ProcessClose() override;

        void ProcessStart() override;

        void ProcessStop() override;

        void ProcessPacket(MediaPacket pkt) override;

        int ModuleOpen(json &cfg) override;

        void ModuleClose() override;

        int ModuleProcessPacket(MediaPacket pkt) override;

    private:
        void MessageProcess(MediaPacket pkt) override;

    private:
        int mStatus;

        MediaBufferPool *pool;

    };

}


#endif //ANDROID_VIDEOFILTER_H
