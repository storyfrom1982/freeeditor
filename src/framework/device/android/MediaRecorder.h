//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <AudioSource.h>
#include <AudioEncoder.h>
#include <MediaProtocol.h>
#include <MessageProcessor.h>
#include <MediaModule.h>
#include "../../media/VideoSource.h"
#include "../../media/VideoRenderer.h"

namespace freee{

    class MediaRecorder : public MessageContext, MediaChainImpl {

    public:
        MediaRecorder();
        ~MediaRecorder();

    private:
        void onRecvMessage(SmartMsg msg) override;

    protected:
        void ProcessMessage(SmartPkt pkt) override;

    private:
        void StartPreview(SmartPkt pkt);
        void Open(SmartPkt pkt);
        void Close();
        void Start();
        void Stop();
        void StartRecord();
        void StopRecord();
        void StopPreview();

        json &GetMediaConfig(MediaChain *chain) override;

    private:
        int mStatus;

        bool isRecording;
        bool isPreviewing;


        AudioSource *mAudioSource;
        VideoSource *mVideoSource;
        VideoRenderer *mVideoRenderer;
    };
}



#endif //ANDROID_MEDIARECORD_H
