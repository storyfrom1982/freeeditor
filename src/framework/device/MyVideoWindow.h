//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


#include <MessageContext.h>
#include "MyVideoRenderer.h"

#ifdef __ANDROID__

#endif


namespace freee {

    class MyVideoWindow : public MessageContext {

    public:

        MyVideoWindow();

        virtual ~MyVideoWindow();

        bool IsReady();

        void RegisterCallback(MyVideoRenderer *callback);

        void* GetNativeWindow();

        int GetWindowWidth();

        int GetWindowHeight();

        void GetWindowSize(int *w, int *h);

        void onRecvMessage(MediaPacket pkt) override;

        MediaPacket onObtainMessage(int key) override;

    private:

        bool isReady;

        void *mWindowHolder;
        void *mNativeWindow;

        MyVideoRenderer *mCallback;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
