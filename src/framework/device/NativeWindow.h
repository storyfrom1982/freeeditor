//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


#include <MessageContext.h>
#include "OpenGLESRender.h"

#ifdef __ANDROID__

#endif


namespace freee {

    class NativeWindow : public MessageContext {

    public:

        NativeWindow();

        virtual ~NativeWindow();

        bool IsReady();

        void ConnectContextHandler(MessageContext *contextHandler) override;

        void SetStatusCallback(MessageContext *callback);

        void* GetWindowHandler();

        void GetWindowSize(int *w, int *h);

        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    private:

        bool isReady;

        void *window;
        void *windowHandler;

        MessageContext *statusCallback;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
