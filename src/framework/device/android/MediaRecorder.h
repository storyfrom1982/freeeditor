//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <AudioSource.h>
#include <VideoEncoder.h>
#include <MessageProcessor.h>
#include <MediaModule.h>
#include "../../media/VideoSource.h"
#include "../../media/VideoRenderer.h"
#include "../../media/VideoFilter.h"

namespace freee{

class MediaRecorder : public MessageContext, MediaChainImpl, MediaChain::EventCallback {

public:
        MediaRecorder();
        ~MediaRecorder();

    private:
        void onRecvMessage(SmartPkt pkt) override;

    protected:
        void MessageProcess(SmartPkt pkt) override;

    private:
        void StartPreview(SmartPkt pkt);
        void Open(SmartPkt pkt);
        void Close();
        void Start();
        void Stop();
        void StartRecord();
        void StopRecord();
        void StopPreview();

        void FinalClearVideoChain();
        void onEvent(MediaChain *chain, SmartPkt pkt) override;

        json &GetConfig(MediaChain *chain) override;

    private:
        int mStatus;

        bool isRecording;
        bool isPreviewing;


        AudioSource *mAudioSource;
        VideoFilter *mVideoFilter;
        VideoSource *mVideoSource;
        VideoRenderer *mVideoRenderer;
        VideoEncoder *mVideoEncoder;
    };
}



#endif //ANDROID_MEDIARECORD_H
