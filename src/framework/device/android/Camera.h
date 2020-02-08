//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_ANDROIDCAMERA_H
#define ANDROID_ANDROIDCAMERA_H


#include <VideoSource.h>


namespace freee {

    class Camera : public VideoSource {

    public:
        Camera(MessageContext *ctx);
        ~Camera();

        virtual void openSource(json cfg) override ;
        virtual void closeSource() override ;

        virtual void startCapture() override ;
        virtual void stopCapture() override ;

    protected:

    public:
        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    };
}



#endif //ANDROID_ANDROIDCAMERA_H
