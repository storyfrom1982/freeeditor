//
// Created by yongge on 19-7-4.
//

#include "VideoWindow.h"

#ifdef __ANDROID__
#include <JNIContext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <MConfig.h>

#endif

using namespace freee;


enum {
    OnPutMsg_WindowError = 0,
    OnPutMsg_WindowCreated = 1,
    OnPutMsg_WindowChanged = 2,
    OnPutMsg_WindowDestroyed = 3
};

enum {
    PutMsg_WindowError = 0,
    PutMsg_RegisterCallback = 1
};

VideoWindow::VideoWindow() {
    isReady = false;
    mWindowHolder = NULL;
    mNativeWindow = NULL;
    SetContextName("VideoWindow");
}

VideoWindow::~VideoWindow() {
#ifdef __ANDROID__
    if (mNativeWindow){
        ANativeWindow_release((ANativeWindow*)mNativeWindow);
    }
    if (mWindowHolder){
        JniEnv env;
        env->DeleteGlobalRef((jobject)mWindowHolder);
    }
#endif
}

void *VideoWindow::GetNativeWindow() {
    return mNativeWindow;
}

void VideoWindow::GetWindowSize(int *w, int *h) {
#ifdef __ANDROID__
    *w = ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
    *h = ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
#endif
}

int VideoWindow::GetWindowWidth() {
#ifdef __ANDROID__
    return ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
#endif
}

int VideoWindow::GetWindowHeight() {
#ifdef __ANDROID__
    return ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
#endif
}

void VideoWindow::OnPutMessage(sr_message_t msg) {
    if (msg.key == OnPutMsg_WindowCreated){
        mWindowHolder = msg.ptr;
#ifdef __ANDROID__
        JniEnv env;
        mNativeWindow = ANativeWindow_fromSurface(env.m_pEnv, (jobject)mWindowHolder);
        if (mCallback){
            msg.key = OpenGLESRender_SurfaceCreated;
            msg.ptr = this;
            SrMessage b;
            b.frame.key = OpenGLESRender_SurfaceCreated;
            b.frame.data = reinterpret_cast<uint8_t *>(this);
            mCallback->OnPutMessage(b);
        }
#endif
    }else if (msg.key == OnPutMsg_WindowChanged){
        msg.key = OpenGLESRender_SurfaceCreated;
        msg.ptr = this;
        SrMessage b;
        b.frame.key = OpenGLESRender_SurfaceCreated;
        b.frame.data = reinterpret_cast<uint8_t *>(this);
        mCallback->OnPutMessage(b);
    }else if (msg.key == OnPutMsg_WindowDestroyed){
        msg.key = OpenGLESRender_SurfaceDestroyed;
        msg.ptr = this;
        SrMessage b;
        b.frame.key = OpenGLESRender_SurfaceDestroyed;
        b.frame.data = reinterpret_cast<uint8_t *>(this);
        mCallback->OnPutMessage(b);
    }
}

sr_message_t VideoWindow::OnGetMessage(sr_message_t msg) {
    return MessageContext::OnGetMessage(msg);
}

void VideoWindow::RegisterCallback(VideoRenderer *callback) {
    mCallback = callback;
    sr_message_t msg = __sr_null_msg;
    msg.key = PutMsg_RegisterCallback;
    PutMessage(msg);
    isReady = true;
    LOGD("VideoWindow::RegisterCallback: %d\n", __is_true(isReady));
}

bool VideoWindow::IsReady() {
    return isReady;
}
