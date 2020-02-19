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
    mWindowHolder = nullptr;
    mNativeWindow = nullptr;
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

void VideoWindow::onReceiveMessage(SrPkt pkt) {
    if (pkt.msg.key == OnPutMsg_WindowCreated){
        mWindowHolder = pkt.msg.obj;
#ifdef __ANDROID__
        JniEnv env;
        mWindowHolder = env->NewGlobalRef(static_cast<jobject>(pkt.msg.obj));
        mNativeWindow = ANativeWindow_fromSurface(env.m_pEnv, (jobject)mWindowHolder);
        if (mCallback){
            pkt.msg.key = OpenGLESRender_SurfaceCreated;
            pkt.msg.ptr = this;
            mCallback->OnPutMessage(pkt);
        }
#endif
    }else if (pkt.msg.key == OnPutMsg_WindowChanged){
        pkt.msg.key = OpenGLESRender_SurfaceCreated;
        pkt.msg.ptr = this;
        mCallback->OnPutMessage(pkt);
    }else if (pkt.msg.key == OnPutMsg_WindowDestroyed){
        pkt.msg.key = OpenGLESRender_SurfaceDestroyed;
        pkt.msg.ptr = this;
        mCallback->OnPutMessage(pkt);
    }
}

SrPkt VideoWindow::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void VideoWindow::RegisterCallback(VideoRenderer *callback) {
    mCallback = callback;
    SrPkt pkt;
    pkt.msg.key = PutMsg_RegisterCallback;
    SendMessage(pkt);
    isReady = true;
    LOGD("VideoWindow::RegisterCallback: %d\n", __is_true(isReady));
}

bool VideoWindow::IsReady() {
    return isReady;
}
