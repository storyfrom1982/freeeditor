//
// Created by yongge on 20-2-22.
//

#ifndef ANDROID_VIDEOSURFACE_H
#define ANDROID_VIDEOSURFACE_H


#include <MessageContext.h>


namespace freee {


    class VideoWindow : public MessageContext {

    public:
        VideoWindow(MessageContext *context);
        ~VideoWindow();

        int width();
        int height();
        void *window();

        class VideoSurfaceCallback {
        public:
            virtual void onSurfaceCreated(void *ptr) = 0;
            virtual void onSurfaceChanged() = 0;
            virtual void onSurfaceDestroyed() = 0;
        };

        void SetCallback(VideoSurfaceCallback *callback);

        void onRecvMessage(SmartPkt pkt) override;

    private:
        void *mWindowHolder;
        void *mNativeWindow;

        Lock mLock;
        VideoSurfaceCallback *mCallback;
    };


}


#endif //ANDROID_VIDEOSURFACE_H
