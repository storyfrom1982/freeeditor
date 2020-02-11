//
// Created by yongge on 19-7-4.
//

#include "NativeWindow.h"

#ifdef __ANDROID__
#include <JNIContext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <MConfig.h>

#endif

using namespace freee;


NativeWindow::NativeWindow() {
    isReady = false;
    window = NULL;
    windowHandler = NULL;
    SetContextName("NativeWindow");
}

NativeWindow::~NativeWindow() {
#ifdef __ANDROID__
    if (windowHandler){
        ANativeWindow_release((ANativeWindow*)windowHandler);
    }
    if (window){
        JniEnv env;
        env->DeleteGlobalRef((jobject)window);
    }
#endif
}

void *NativeWindow::GetWindowHandler() {
    return windowHandler;
}

void NativeWindow::GetWindowSize(int *w, int *h) {
#ifdef __ANDROID__
    *w = ANativeWindow_getWidth((ANativeWindow*)windowHandler);
    *h = ANativeWindow_getHeight((ANativeWindow*)windowHandler);
#endif
}

void NativeWindow::OnPutMessage(sr_message_t msg) {
    if (msg.key == 1){
        window = msg.ptr;
#ifdef __ANDROID__
        JniEnv env;
        windowHandler = ANativeWindow_fromSurface(env.m_pEnv, (jobject)window);
        if (statusCallback){
            msg.key = OpenGLESRender_SurfaceCreated;
            msg.ptr = this;
            statusCallback->OnPutMessage(msg);
        }
#endif
    }else if (msg.key == 2){
        if (msg.ptr != NULL){
//            json  js = json::parse((char *)(msg.ptr));
//            LOGD("reszie: %s\n", js.dump().c_str());
            free(msg.ptr);
        }

    }else if (msg.key == 3){
        msg.key = OpenGLESRender_SurfaceDestroyed;
        msg.ptr = this;
        statusCallback->OnPutMessage(msg);
    }
}

sr_message_t NativeWindow::OnGetMessage(sr_message_t msg) {
    return MessageContext::OnGetMessage(msg);
}

void NativeWindow::SetStatusCallback(MessageContext *callback) {
    statusCallback = callback;
    sr_message_t msg = __sr_null_msg;
    msg.key = 1;
    PutMessage(msg);
    isReady = true;
    LOGD("NativeWindow::SetStatusCallback: %d\n", __is_true(isReady));
}

bool NativeWindow::IsReady() {
    return isReady;
}

void NativeWindow::ConnectContextHandler(MessageContext *contextHandler) {
    MessageContext::ConnectContextHandler(contextHandler);
}
