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

        void SetRenderer(MessageContext *context);

        void* getWindowHandler();

        void getWindowSize(int *w, int *h);

        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    private:

        void *window;
        void *windowHandler;

        MessageContext *context;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
