//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <DeviceContext.h>
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
    };

    class MediaRecord : public DeviceContext{

    public:
        MediaRecord(DeviceContext *ctx);
        ~MediaRecord();

    protected:
        int onPutObject(int type, void *obj) override;

        void *onGetObject(int type) override;

        int onPutMessage(int cmd, std::string msg) override;

        std::string onGetMessage(int cmd) override;

        int onPutData(void *data, int size) override;

        void *onGetBuffer() override;


    private:
        void init(sr_msg_t msg);
        void messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg);
        static void messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg);


    private:
        sr_msg_queue_t *m_queue;
        sr_msg_processor_t m_processor;

        VideoSource *videoSource;
        NativeWindow *window;

        OpenGLESRender *glesRender;
    };
}



#endif //ANDROID_MEDIARECORD_H
