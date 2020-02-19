//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <VideoSource.h>
#include <VideoWindow.h>
#include <VideoRenderer.h>
#include <AudioSource.h>
#include <AudioEncoder.h>
#include <MediaProtocol.h>

namespace freee{


    enum {
        Record_SetConfig = 0,
        Record_StartCapture,
        Record_StartRecord,
        Record_StopCapture,
        Record_StopRecord,
        Record_ChangeCameraConfig,
        Record_ChangeEncodeConfig,
        Record_StartPreview,
        Record_DrawPicture,
        Record_SetUrl,
    };

    class MediaRecorder :public MessageContext, SrMessageQueue {

    public:
        MediaRecorder();

        void onReceiveMessage(SrPkt msg) override;

        ~MediaRecorder();

    protected:
    private:
        void MessageProcessor(SrPkt pkt) override;

    private:
        void Initialize(SrPkt pkt);
        void StartPreview(SrPkt pkt);

    private:

        AudioSource *audioSource;
        VideoSource *videoSource;
        VideoEncoder *videoEncoder;
        AudioEncoder *audioEncoder;
        MediaProtocol *mediaProtocol;

        json mConfig;
    };
}



#endif //ANDROID_MEDIARECORD_H
