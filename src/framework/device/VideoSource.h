//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <MConfig.h>
#include <MessageContext.h>
#include "OpenGLESRender.h"


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

        static VideoSource* openVideoSource(MessageContext *ctx);

        VideoSource();
        virtual ~VideoSource();

        void setEncoder(MessageContext *ctx);

        virtual void openSource(json cfg) = 0;
        virtual void closeSource() = 0;

        virtual void startCapture() = 0;
        virtual void stopCapture() = 0;


    protected:

        void processData(void *data, int size);
        void processData(sr_message_t msg);

    private:

        MessageContext *outputCtx;

    };


}


#endif //ANDROID_CAMERA_H
