//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


#include <MessageContext.h>
#include "VideoRenderer.h"

#ifdef __ANDROID__

#endif


namespace freee {

    class VideoWindow : public MessageContext {

    public:

        VideoWindow();

        virtual ~VideoWindow();

        bool IsReady();

        void RegisterCallback(VideoRenderer *callback);

        void* GetNativeWindow();

        int GetWindowWidth();

        int GetWindowHeight();

        void GetWindowSize(int *w, int *h);

        void onReceiveMessage(MediaPacket pkt) override;

        MediaPacket onObtainMessage(int key) override;

    private:

        bool isReady;

        void *mWindowHolder;
        void *mNativeWindow;

        VideoRenderer *mCallback;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
