//
// Created by yongge on 20-2-22.
//

#ifndef ANDROID_VIDEOSURFACE_H
#define ANDROID_VIDEOSURFACE_H


#include "../message/MessageContext.h"


namespace freee {


    class VideoWindow : public MessageContext {

    public:
        VideoWindow(MessageContext *context);
        ~VideoWindow();

        int width();
        int height();
        void *window();

        class VideoWindowCallback {
        public:
            virtual void onSurfaceCreated(void *ptr) = 0;
            virtual void onSurfaceChanged(int width, int height) = 0;
            virtual void onSurfaceDestroyed() = 0;
        };

        void SetCallback(VideoWindowCallback *callback);

        void onRecvMessage(Message pkt) override;

    private:
        void *mWindowHolder;
        void *mNativeWindow;
        Lock mLock;
        VideoWindowCallback *mCallback;
    };


}


#endif //ANDROID_VIDEOSURFACE_H
