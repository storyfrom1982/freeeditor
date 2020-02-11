//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <MConfig.h>
#include <MessageContext.h>
#include "OpenGLESRender.h"
#include "NativeWindow.h"
#include "VideoEncoder.h"


namespace freee {


    enum {
        VideoSource_Open = 0,
        VideoSource_Start,
        VideoSource_Stop,
        VideoSource_Close,
        VideoSource_Release
    };


    class VideoSource : public MessageContext {

    public:

        static VideoSource* CreateVideoSource();

        VideoSource();
        virtual ~VideoSource();

        void SetEncoder(VideoEncoder *videoEncoder);
        void SetWindow(MessageContext *windowContext);

        void StartPreview();
        void StopPreview();

        virtual void Open(json cfg) = 0;
        virtual void closeSource() = 0;

        virtual void startCapture() = 0;
        virtual void stopCapture() = 0;


    protected:

        void processData(void *data, int size);
        void processData(sr_message_t msg);

    private:

        bool isPreview;

        VideoEncoder *encoder;

        NativeWindow *window;
        OpenGLESRender *render;

        sr_buffer_pool_t *pool;

    };


}


#endif //ANDROID_CAMERA_H
