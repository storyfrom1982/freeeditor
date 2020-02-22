//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <MyVideoSource.h>
#include <MyVideoWindow.h>
#include <MyVideoRenderer.h>
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
        void onRecvMessage(MediaPacket pkt) override;

    protected:
        void ProcessMessage(MediaPacket pkt) override;

    private:
        void StartPreview(MediaPacket pkt);
        void Open(MediaPacket pkt);
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

//        MyVideoSource *videoSource;

        VideoSource *mVideoSource;
        VideoRenderer *mVideoRenderer;
    };
}



#endif //ANDROID_MEDIARECORD_H
