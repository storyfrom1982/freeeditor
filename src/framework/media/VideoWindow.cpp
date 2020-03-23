//
// Created by yongge on 20-2-22.
//

#include <android/native_window.h>
#include <JNIContext.h>
#include <android/native_window_jni.h>
#include <MessagePool.h>
#include <MConfig.h>
#include "VideoWindow.h"



using namespace freee;


enum {
    SendMsg_Error = 0,
    SendMsg_WindowCreated = 1,
    SendMsg_WindowDestroyed = 2
};

enum {
    RecvMsg_Error = 0,
    RecvMsg_SurfaceCreated = 1,
    RecvMsg_SurfaceChanged = 2,
    RecvMsg_SurfaceDestroyed = 3
};


VideoWindow::VideoWindow(MessageContext *context) : MessageContext("VideoWindow") {
    mWindowHolder = nullptr;
    mNativeWindow = nullptr;
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

void VideoWindow::onRecvMessage(Message pkt) {
    if (pkt.GetKey() == RecvMsg_SurfaceCreated){
#ifdef __ANDROID__
        JniEnv env;
        AutoLock lock(mLock);
//        mWindowHolder = pkt.GetPtr();
        mWindowHolder = env->NewGlobalRef(static_cast<jobject>(pkt.GetPtr()));
        mNativeWindow = ANativeWindow_fromSurface(env.m_pEnv, (jobject)mWindowHolder);
        if (mCallback){
            mCallback->onSurfaceCreated(mNativeWindow);
        }
#endif
    }else if (pkt.GetKey() == RecvMsg_SurfaceChanged){
        AutoLock lock(mLock);
        if (mCallback){
            LOGD("RecvMsg_SurfaceChanged[%s]\n", pkt.GetString().c_str());
            json js = json::parse(pkt.GetString());
            mCallback->onSurfaceChanged(js["width"], js["height"]);
        }
    }else if (pkt.GetKey() == RecvMsg_SurfaceDestroyed){
        AutoLock lock(mLock);
        if (mCallback){
            mCallback->onSurfaceDestroyed();
        }
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
}

void VideoWindow::SetCallback(VideoWindow::VideoWindowCallback *callback) {
    AutoLock lock(mLock);
    mCallback = callback;
    if (mCallback){
        SendMessage(Message(SendMsg_WindowCreated));
    }else {
        SendMessage(Message(SendMsg_WindowDestroyed));
    }
}
