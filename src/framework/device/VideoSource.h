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


    class VideoSource : public MessageContext {

    public:

//        static VideoSource* CreateVideoSource();

        VideoSource();
        virtual ~VideoSource() override ;

        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

        void SetEncoder(VideoEncoder *videoEncoder);
        void SetWindow(MessageContext *windowContext);

        void StartPreview();
        void StopPreview();

        virtual void Open(json cfg);
        virtual void Close();

        virtual void Start();
        virtual void Stop();


    protected:

        void processData(void *data, int size);
        void processData(sr_message_t msg);

    private:

        void Release();

    private:

        bool isPreview;
        bool isClosed;

        VideoEncoder *encoder;

        NativeWindow *window;
        OpenGLESRender *render;

        sr_buffer_pool_t *pool;

    };


}


#endif //ANDROID_CAMERA_H
