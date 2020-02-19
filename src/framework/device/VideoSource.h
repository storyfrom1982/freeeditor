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

        void onReceiveMessage(SrPkt msg) override;

        SrPkt onObtainMessage(int key) override;

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

    private:

        void Release();
        void updateConfig(SrPkt pkt);

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
