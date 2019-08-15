//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_ANDROIDCAMERA_H
#define ANDROID_ANDROIDCAMERA_H


#include <VideoSource.h>


namespace freee {

    class Camera : public VideoSource {

    public:
        Camera(StreamProcessor *listener);
        ~Camera();

        virtual void openSource(json cfg) override ;
        virtual void closeSource() override ;

        virtual void startCapture() override ;
        virtual void stopCapture() override ;

        virtual sr_msg_t requestFromInputStream(sr_msg_t msg) override ;
        virtual sr_msg_t requestFromOutputStream(sr_msg_t msg) override ;
    };
}



#endif //ANDROID_ANDROIDCAMERA_H
