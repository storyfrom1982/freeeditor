//
// Created by yongge on 19-5-24.
//

#ifndef ANDROID_RECORDER_H
#define ANDROID_RECORDER_H


#include <IMsgListener.h>
#include <MConfig.h>
#include <VideoSource.h>
#include <NativeWindow.h>
#include <VideoRenderer.h>


namespace freee {

    class Editor : public IMsgListener {

    public:

        Editor(sr_msg_t msg);
        ~Editor();


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

        NativeWindow *nativeWindow;
        VideoRenderer *videoRenderer;
        GLObject *glObject;
    };


}


#endif //ANDROID_RECORDER_H
