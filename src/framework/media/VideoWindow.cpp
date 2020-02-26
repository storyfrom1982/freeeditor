//
// Created by yongge on 20-2-22.
//

#include <android/native_window.h>
#include <JNIContext.h>
#include <android/native_window_jni.h>
#include <BufferPool.h>
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
        mNativeWindow = nullptr;
    }
    if (mWindowHolder){
        JniEnv env;
        env->DeleteGlobalRef((jobject)mWindowHolder);
        mWindowHolder = nullptr;
    }
#endif
}

void *VideoWindow::window() {
    AutoLock lock(mLock);
    return mNativeWindow;
}

int VideoWindow::width() {
    AutoLock lock(mLock);
#ifdef __ANDROID__
    if (mNativeWindow){
        return ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
    }
#endif
    return 0;
}

int VideoWindow::height() {
    AutoLock lock(mLock);
#ifdef __ANDROID__
    if (mNativeWindow){
        return ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
    }
#endif
    return 0;
}

void VideoWindow::onRecvMessage(SmartPkt pkt) {
    if (pkt.msg.key == RecvMsg_SurfaceCreated){
#ifdef __ANDROID__
        JniEnv env;
        AutoLock lock(mLock);
        mWindowHolder = pkt.msg.troubledPtr;
        mWindowHolder = env->NewGlobalRef(static_cast<jobject>(pkt.msg.troubledPtr));
        mNativeWindow = ANativeWindow_fromSurface(env.m_pEnv, (jobject)mWindowHolder);
        if (mCallback){
            mCallback->onSurfaceCreated(mNativeWindow);
        }
#endif
    }else if (pkt.msg.key == RecvMsg_SurfaceChanged){
        AutoLock lock(mLock);
        if (mCallback){
            mCallback->onSurfaceChanged();
        }
    }else if (pkt.msg.key == RecvMsg_SurfaceDestroyed){
        AutoLock lock(mLock);
#ifdef __ANDROID__
        if (mNativeWindow){
            ANativeWindow_release((ANativeWindow*)mNativeWindow);
            mNativeWindow = nullptr;
        }
        if (mWindowHolder){
            JniEnv env;
            env->DeleteGlobalRef((jobject)mWindowHolder);
            mWindowHolder = nullptr;
        }
#endif
        if (mCallback){
            mCallback->onSurfaceDestroyed();
        }
    }
}

void VideoWindow::SetCallback(VideoWindow::VideoSurfaceCallback *callback) {
    AutoLock lock(mLock);
    mCallback = callback;
    SendMessage(SmartPkt(SendMsg_RegisterCallback));
}
