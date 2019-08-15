//
// Created by yongge on 19-5-24.
//

#ifndef ANDROID_RECORDER_H
#define ANDROID_RECORDER_H


#include <StreamProcessor.h>
#include <MConfig.h>
#include <VideoSource.h>
#include <NativeWindow.h>
#include <VideoRenderer.h>
#include "StreamCapture.h"


namespace freee {

    class Editor : public StreamCapture {

    public:

        Editor(sr_msg_t msg);
        ~Editor();


    private:

        virtual sr_msg_t requestFromInputStream(sr_msg_t msg) override ;
        virtual sr_msg_t requestFromOutputStream(sr_msg_t msg) override ;
        virtual void messageFromInputStream(sr_msg_t msg) override ;
        virtual void messageFromOutputStream(sr_msg_t msg) override ;

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
