//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <MyVideoSource.h>
#include <VideoWindow.h>
#include <VideoRenderer.h>
#include <AudioSource.h>
#include <AudioEncoder.h>
#include <MediaProtocol.h>
#include <MediaProcessor.h>
#include <MediaModule.h>

namespace freee{

    class MediaRecorder : public MessageContext, MediaChainImpl, MediaProcessor {

    public:
        MediaRecorder();
        ~MediaRecorder();

    private:
        void onReceiveMessage(MediaPacket pkt) override;

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

    private:
        int mStatus;

        bool isRecording;
        bool isPreviewing;

        MyVideoSource *videoSource;
    };
}



#endif //ANDROID_MEDIARECORD_H
