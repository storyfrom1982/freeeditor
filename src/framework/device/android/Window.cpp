//
// Created by yongge on 19-7-4.
//

#include "Window.h"

using namespace freee;

Window::Window() {

}

Window::~Window() {

}

void *Window::getWindowHandler() {
    return NativeWindow::getWindowHandler();
}
