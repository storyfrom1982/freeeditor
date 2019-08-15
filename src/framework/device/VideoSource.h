//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <StreamProcessor.h>
#include <MConfig.h>


namespace freee {


    class VideoSource : public StreamProcessor {

    public:

        static VideoSource* openVideoSource(StreamProcessor *listener);

        VideoSource(StreamProcessor *listener);
        virtual ~VideoSource();

        virtual void openSource(json cfg) = 0;
        virtual void closeSource() = 0;

        virtual void startCapture() = 0;
        virtual void stopCapture() = 0;


    protected:

        int imageFilter(void *src, int src_w, int src_h, int src_fmt,
                void *dst, int dst_w, int dst_h, int dst_fmt, int rotate);


    protected:

        json m_videoConfig;
        StreamProcessor *m_listener;

    };


}


#endif //ANDROID_CAMERA_H
