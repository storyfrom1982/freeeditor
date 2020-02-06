//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <MConfig.h>
#include <DeviceContext.h>
#include "OpenGLESRender.h"


namespace freee {


    enum {
        VideoSource_Open = 0,
        VideoSource_Start,
        VideoSource_Stop,
        VideoSource_Close
    };


    class VideoSource : public DeviceContext {

    public:

        static VideoSource* openVideoSource(DeviceContext *ctx);

        VideoSource(DeviceContext *ctx);
        virtual ~VideoSource();

        void setEncoder(DeviceContext *ctx);

        virtual void openSource(json cfg) = 0;
        virtual void closeSource() = 0;

        virtual void startCapture() = 0;
        virtual void stopCapture() = 0;


    protected:

        void processData(void *data, int size);

        virtual int imageFilter(void *src, int src_w, int src_h, int src_fmt,
                void *dst, int dst_w, int dst_h, int dst_fmt, int rotate);

    private:

        DeviceContext *outputCtx;

    };


}


#endif //ANDROID_CAMERA_H
