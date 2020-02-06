//
// Created by yongge on 19-7-4.
//

#include "NativeWindow.h"

#ifdef __ANDROID__
#include <JNIHandler.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#endif

using namespace freee;


NativeWindow::NativeWindow(void *p) {
#ifdef __ANDROID__
    JniEnv env;
    windowHanler = ANativeWindow_fromSurface(env.m_pEnv, (jobject)p);
#endif
}

void *NativeWindow::getWindowHandler() {
    return windowHanler;
}

void NativeWindow::getWindowSize(int *w, int *h) {
#ifdef __ANDROID__
    *w = ANativeWindow_getWidth((ANativeWindow*)windowHanler);
    *h = ANativeWindow_getHeight((ANativeWindow*)windowHanler);
#endif
}
