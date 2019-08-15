//
// Created by yongge on 19-7-4.
//

#include "Window.h"

#include <SDKJNI.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <StreamProcessor.h>


using namespace freee;

static jobject g_source;

Window::Window(void *windowHandler) {
    LOGD("====================surface addr %p\n", windowHandler);
    JniEnv env;
//    g_source = env->NewGlobalRef((jobject)windowHandler);
    g_source = (jobject)windowHandler;
    ANativeWindow* hWnd = ANativeWindow_fromSurface(env.m_pEnv, g_source);
    mWindowHandler = hWnd;
//    env->DeleteGlobalRef(g_source);
}

Window::~Window() {

}

void *Window::getWindowHandler() {
    return mWindowHandler;
}

void Window::getWindowSize(int *w, int *h) {
    if (w && h){
        *w = ANativeWindow_getWidth((ANativeWindow*)mWindowHandler);
        *h = ANativeWindow_getHeight((ANativeWindow*)mWindowHandler);
    }
}
