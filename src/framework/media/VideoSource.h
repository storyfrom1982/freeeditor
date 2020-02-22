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

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, MediaPacket pkt) override;

        void Control(MediaChain *chain, MediaPacket pkt) override;

    private:
        void onRecvMessage(MediaPacket pkt) override;

        void UpdateMediaConfig(MediaPacket pkt);

    private:
        int mStatus;

        int mSrcRotation;
        int mSrcWidth, mSrcHeight;
        int mCodecWidth, mCodecHeight;

        Lock mLock;
        size_t mBufferSize;
        MediaBufferPool *mPool;
    };

}


#endif //ANDROID_VIDEOSOURCE_H
