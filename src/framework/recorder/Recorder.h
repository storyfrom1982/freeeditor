//
// Created by yongge on 19-5-24.
//

#ifndef ANDROID_RECORDER_H
#define ANDROID_RECORDER_H


#include <IMsgListener.h>
#include <MConfig.h>
#include <VideoSource.h>


namespace freee {


    enum
    {

        MSG_RecvReq_LoadConfig = 0,
        MSG_RecvReq_SaveConfig,

        MSG_RecvCmd_SetVideoSource,
        MSG_RecvCmd_RemoveVideoSource,
        MSG_RecvCmd_SetAudioSource,
        MSG_RecvCmd_RemoveAudioSource,
        MSG_RecvCmd_StartPreview,
        MSG_RecvCmd_StopPreview,
        MSG_RecvCmd_StartPushStream,
        MSG_RecvCmd_StopPushStream,
        MSG_RecvCmd_SetVideoView,
        MSG_RecvCmd_EnableAudioStream,
        MSG_RecvCmd_DisableAudioStream,
        MSG_RecvCmd_EnableVideoStream,
        MSG_RecvCmd_DisableVideoStream,
        MSG_RecvCmd_OpenCamera,
        MSG_RecvCmd_CloseCamera,
        MSG_RecvCmd_OpenMicrophone,
        MSG_RecvCmd_CloseMicrophone,

    };


    enum
    {
        MSG_SendReq_GetCamera = 0,
        MSG_SendReq_GetMicrophone,
    };


    class Recorder : public IMsgListener {

    public:

        Recorder(sr_msg_t msg);
        ~Recorder();


    private:

        virtual sr_msg_t onRequestFromUpstream(sr_msg_t msg) override ;
        virtual sr_msg_t onRequestFromDownstream(sr_msg_t msg) override ;
        virtual void onMessageFromUpstream(sr_msg_t msg) override ;
        virtual void onMessageFromDownstream(sr_msg_t msg) override ;

    private:

        sr_msg_t loadConfig();
        sr_msg_t saveConfig(sr_msg_t msg);

        void setVideoSource(sr_msg_t msg);
        void removeVideoSource();

        void setAudioSource(sr_msg_t msg);
        void removeAudioSource();

        void setVideoView(sr_msg_t msg);
        void startPreview();
        void stopPreview();

        void startPushStream();
        void stopPushStream();

        void messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg);
        static void messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg);




    private:

        json m_config;
        sr_msg_queue_t *m_queue;
        sr_msg_processor_t m_processor;

        VideoSource *m_videoSource;
    };


}


#endif //ANDROID_RECORDER_H
