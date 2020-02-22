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

    protected:
        void MessageOpen(MediaPacket pkt) override;

        void MessageClose(MediaPacket pkt) override;

        void MessageStart(MediaPacket pkt) override;

        void MessageStop(MediaPacket pkt) override;

        void MessagePacket(MediaPacket pkt) override;

        int ModuleImplOpen(json &cfg) override;

        void ModuleImplClose() override;

        int ModuleImplProcessMedia(MediaPacket pkt) override;

    private:
        int mStatus;

        MediaBufferPool *pool;

    };

}


#endif //ANDROID_VIDEOFILTER_H
