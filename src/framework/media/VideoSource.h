//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_VIDEOSOURCE_H
#define ANDROID_VIDEOSOURCE_H


#include <MediaModule.h>
#include <MessageContext.h>

namespace freee {


    class VideoSource : public MediaChainImpl, MessageContext {

    public:
        VideoSource(MessageContext *context = nullptr);
        ~VideoSource();

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, MediaPacket pkt) override;

        int GetMediaType(MediaChain *chain) override;

        int GetMediaNumber(MediaChain *chain) override;

        std::string GetMediaName(MediaChain *chain) override;

        json &GetMediaConfig(MediaChain *chain) override;

    private:
        void onReceiveMessage(MediaPacket pkt) override;

        void onOpened(MediaPacket pkt);

    private:
        json mConfig;

        std::string mModuleName;
    };

}


#endif //ANDROID_VIDEOSOURCE_H
