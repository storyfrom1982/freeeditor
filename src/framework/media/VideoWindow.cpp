//
// Created by yongge on 20-2-22.
//

#include <android/native_window.h>
#include <JNIContext.h>
#include <android/native_window_jni.h>
#include "VideoWindow.h"



using namespace freee;


enum {
    SendMsg_Error = 0,
    SendMsg_RegisterCallback = 1
};

enum {
    RecvMsg_Error = 0,
    RecvMsg_SurfaceCreated = 1,
    RecvMsg_SurfaceChanged = 2,
    RecvMsg_SurfaceDestroyed = 3
};


VideoWindow::VideoWindow(MessageContext *context) {
    mWindowHolder = nullptr;
    mNativeWindow = nullptr;
    SetContextName("VideoWindow");
    ConnectContext(context);
}


VideoWindow::~VideoWindow() {
    DisconnectContext();
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

void *VideoWindow::window() {
    return mNativeWindow;
}

int VideoWindow::width() {
#ifdef __ANDROID__
    return ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
#else
    return 0;
#endif
}

int VideoWindow::height() {
#ifdef __ANDROID__
    return ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
#else
    return 0;
#endif
}

void VideoWindow::onRecvMessage(SmartMsg msg) {
    if (msg.GetKey() == RecvMsg_SurfaceCreated){
        mWindowHolder = msg.GetTroubledPtr();
#ifdef __ANDROID__
        JniEnv env;
        mWindowHolder = env->NewGlobalRef(static_cast<jobject>(msg.GetTroubledPtr()));
        mNativeWindow = ANativeWindow_fromSurface(env.m_pEnv, (jobject)mWindowHolder);
        AutoLock lock(mLock);
        if (mCallback){
            mCallback->onSurfaceCreated(mNativeWindow);
        }
#endif
    }else if (msg.GetKey() == RecvMsg_SurfaceChanged){
        AutoLock lock(mLock);
        if (mCallback){
            mCallback->onSurfaceChanged();
        }
    }else if (msg.GetKey() == RecvMsg_SurfaceDestroyed){
        AutoLock lock(mLock);
        if (mCallback){
            mCallback->onSurfaceDestroyed();
        }
    }
}

void VideoWindow::SetCallback(VideoWindow::VideoSurfaceCallback *callback) {
    AutoLock lock(mLock);
    mCallback = callback;
    SendMessage(SmartMsg(SendMsg_RegisterCallback));
}
