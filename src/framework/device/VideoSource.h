//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <MConfig.h>
#include <MessageContext.h>
#include <SrBufferPool.h>
#include "VideoRenderer.h"
#include "VideoWindow.h"
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

        virtual void Open(json &cfg);
        virtual void Close();

        virtual void Start();
        virtual void Stop();


    protected:

        void processData(void *data, int size);
        void processData(sr_message_t msg);

    private:

        void Release();
        void updateConfig(sr_message_t msg);

    private:

        bool isPreview;
        bool isClosed;

        int mRotation;
        int mInputWidth, mInputHeight;
        int mOutputWidth, mOutputHeight;

        VideoEncoder *encoder;

        VideoWindow *window;
        VideoRenderer *render;

        SrBufferPool *bp;
        sr_buffer_pool_t *pool;

        json mConfig;
    };


}


#endif //ANDROID_CAMERA_H
