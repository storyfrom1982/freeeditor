//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <VideoSource.h>
#include <NativeWindow.h>
#include <OpenGLESRender.h>

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

    class MediaRecord : public MessageContext{

    public:
        MediaRecord();
        ~MediaRecord();

    public:
        void OnPutDataBuffer(sr_message_t msg) override;

//        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    protected:
        void MessageProcessor(sr_message_t msg) override;

    private:
        void init(sr_message_t msg);
        void StartPreview(sr_message_t msg);
        void drawPicture(sr_message_t msg);
//        static void messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg);


    private:
//        sr_message_queue_t *m_queue;
//        sr_message_processor_t m_processor;

        VideoSource *videoSource;
        NativeWindow *window;

        OpenGLESRender *glesRender;
    };
}



#endif //ANDROID_MEDIARECORD_H
