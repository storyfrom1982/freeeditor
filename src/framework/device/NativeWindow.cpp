//
// Created by yongge on 19-7-4.
//

#include "NativeWindow.h"

#ifdef __ANDROID__
#include <JNIContext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#endif

using namespace freee;


NativeWindow::NativeWindow(void *p) {
    window = p;
#ifdef __ANDROID__
    JniEnv env;
    windowHandler = ANativeWindow_fromSurface(env.m_pEnv, (jobject)window);
#endif
}

NativeWindow::~NativeWindow() {
#ifdef __ANDROID__
    ANativeWindow_release((ANativeWindow*)windowHandler);
    JniEnv env;
    env->DeleteGlobalRef((jobject)window);
#endif
}

void *NativeWindow::getWindowHandler() {
    return windowHandler;
}

void NativeWindow::getWindowSize(int *w, int *h) {
#ifdef __ANDROID__
    *w = ANativeWindow_getWidth((ANativeWindow*)windowHandler);
    *h = ANativeWindow_getHeight((ANativeWindow*)windowHandler);
#endif
}
