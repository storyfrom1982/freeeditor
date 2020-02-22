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
        void MessageOpen(SmartPkt pkt) override;

        void MessageClose(SmartPkt pkt) override;

        void MessageStart(SmartPkt pkt) override;

        void MessageStop(SmartPkt pkt) override;

        void MessagePacket(SmartPkt pkt) override;

        int ModuleImplOpen(json &cfg) override;

        void ModuleImplClose() override;

        int ModuleImplProcessMedia(SmartPkt pkt) override;

    private:
        int mStatus;

        BufferPool *pool;

    };

}


#endif //ANDROID_VIDEOFILTER_H
