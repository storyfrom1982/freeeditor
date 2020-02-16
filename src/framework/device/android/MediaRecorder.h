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

    class MediaRecorder : public MessageContext{

    public:
        MediaRecorder();

        void OnPutMessage(sr_message_t msg) override;

        ~MediaRecorder();

    protected:
        void MessageProcessor(sr_message_t msg) override;

    private:
        void Initialize(sr_message_t msg);
        void StartPreview(sr_message_t msg);

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
