//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <VideoSource.h>
#include <VideoWindow.h>
#include <VideoRenderer.h>

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
    };

    class MediaRecorder : public MessageContext{

    public:
        MediaRecorder();
        ~MediaRecorder();

    protected:
        void MessageProcessor(sr_message_t msg) override;

    private:
        void Initialize(sr_message_t msg);
        void StartPreview(sr_message_t msg);

    private:

        VideoSource *videoSource;
        VideoEncoder *videoEncoder;
    };
}



#endif //ANDROID_MEDIARECORD_H
