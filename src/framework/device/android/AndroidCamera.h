//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_ANDROIDCAMERA_H
#define ANDROID_ANDROIDCAMERA_H


#include <VideoSource.h>


namespace freee {

    enum {
        MSG_KEY_SendCmd_OpenSource = 0,
        MSG_KEY_SendCmd_CloseSource,
        MSG_KEY_SendCmd_StartCapture,
        MSG_KEY_SendCmd_StopCapture,
        MSG_KEY_RecvCmd_UpdateVideoConfig,
        MSG_KEY_RecvCmd_PreviewFrame,
    };


    class AndroidCamera : public VideoSource {

    public:
        AndroidCamera(IMsgListener *listener);
        ~AndroidCamera();

        virtual void openSource(json cfg) override ;
        virtual void closeSource() override ;

        virtual void startCapture() override ;
        virtual void stopCapture() override ;

        virtual sr_msg_t onInputRequest(sr_msg_t msg) override ;
        virtual sr_msg_t onOutputRequest(sr_msg_t msg) override ;
    };
}



#endif //ANDROID_ANDROIDCAMERA_H
