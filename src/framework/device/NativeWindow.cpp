//
// Created by yongge on 19-7-4.
//

#include "NativeWindow.h"

#include <IMsgListener.h>

#include <android/Window.h>


using namespace freee;


NativeWindow *NativeWindow::createNativeWindow(void *p) {
    return new Window(p);
}
