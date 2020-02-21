//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_CAMERA_H
#define ANDROID_CAMERA_H


#include <MConfig.h>
#include <MessageContext.h>
#include <MediaBufferPool.h>
#include "VideoRenderer.h"
#include "VideoWindow.h"
#include "VideoEncoder.h"


namespace freee {


    class MyVideoSource : public MessageContext {

    public:

//        static MyVideoSource* CreateVideoSource();

        MyVideoSource();
        virtual ~MyVideoSource() override ;

        void onReceiveMessage(MediaPacket msg) override;

        MediaPacket onObtainMessage(int key) override;

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
        void updateConfig(MediaPacket pkt);

    private:

        bool isPreview;
        bool isClosed;

        int mRotation;
        int mInputWidth, mInputHeight;
        int mOutputWidth, mOutputHeight;


        Lock mLock;
        VideoEncoder *encoder;

        VideoWindow *window;
        VideoRenderer *render;

        MediaBufferPool *bp;
        sr_buffer_pool_t *pool;

        json mConfig;
    };


}


#endif //ANDROID_CAMERA_H
