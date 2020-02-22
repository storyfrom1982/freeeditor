//
// Created by yongge on 19-7-4.
//

#include "MyVideoWindow.h"

#ifdef __ANDROID__
#include <JNIContext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <MConfig.h>

#endif

using namespace freee;


enum {
    RecvMsg_Error = 0,
    RecvMsg_SurfaceCreated = 1,
    RecvMsg_SurfaceChanged = 2,
    RecvMsg_SurfaceDestroyed = 3
};

enum {
    SendMsg_Error = 0,
    SendMsg_RegisterCallback = 1
};

MyVideoWindow::MyVideoWindow() {
    isReady = false;
    mWindowHolder = nullptr;
    mNativeWindow = nullptr;
    SetContextName("MyVideoWindow");
}

MyVideoWindow::~MyVideoWindow() {
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

void *MyVideoWindow::GetNativeWindow() {
    return mNativeWindow;
}

void MyVideoWindow::GetWindowSize(int *w, int *h) {
#ifdef __ANDROID__
    *w = ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
    *h = ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
#endif
}

int MyVideoWindow::GetWindowWidth() {
#ifdef __ANDROID__
    return ANativeWindow_getWidth((ANativeWindow*)mNativeWindow);
#endif
}

int MyVideoWindow::GetWindowHeight() {
#ifdef __ANDROID__
    return ANativeWindow_getHeight((ANativeWindow*)mNativeWindow);
#endif
}

void MyVideoWindow::onRecvMessage(SmartPkt pkt) {
    if (pkt.msg.key == RecvMsg_SurfaceCreated){
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
    }else if (pkt.msg.key == RecvMsg_SurfaceChanged){
        pkt.msg.key = OpenGLESRender_SurfaceCreated;
        pkt.msg.ptr = this;
        mCallback->OnPutMessage(pkt);
    }else if (pkt.msg.key == RecvMsg_SurfaceDestroyed){
        pkt.msg.key = OpenGLESRender_SurfaceDestroyed;
        pkt.msg.ptr = this;
        mCallback->OnPutMessage(pkt);
    }
}

SmartPkt MyVideoWindow::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void MyVideoWindow::RegisterCallback(MyVideoRenderer *callback) {
    mCallback = callback;
    SmartPkt pkt;
    pkt.msg.key = SendMsg_RegisterCallback;
    SendMessage(pkt);
    isReady = true;
    LOGD("MyVideoWindow::RegisterCallback: %d\n", __is_true(isReady));
}

bool MyVideoWindow::IsReady() {
    return isReady;
}
