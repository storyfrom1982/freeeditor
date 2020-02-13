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

        void RegisterCallback(MessageContext *callback);

        void* GetNativeWindow();

        int GetWindowWidth();

        int GetWindowHeight();

        void GetWindowSize(int *w, int *h);

        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    private:

        bool isReady;

        void *mWindowHolder;
        void *mNativeWindow;

        MessageContext *mCallback;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
