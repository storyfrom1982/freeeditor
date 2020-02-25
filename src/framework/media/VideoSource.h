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

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;


    private:
        void onRecvMessage(SmartMsg msg) override;

        void UpdateMediaConfig(SmartMsg msg);

    private:
        int mStatus;

        int mSrcRotation;
        int mSrcWidth, mSrcHeight;
        int mCodecWidth, mCodecHeight;
        int mSrcImageFormat, mCodecImageFormat;

        Lock mLock;
        size_t mBufferSize;
        BufferPool *mBufferPool;
    };

}


#endif //ANDROID_VIDEOSOURCE_H
