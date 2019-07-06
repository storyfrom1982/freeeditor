//
// Created by yongge on 19-7-4.
//

#include "AndroidWindow.h"

using namespace freee;

AndroidWindow::AndroidWindow() {

}

AndroidWindow::~AndroidWindow() {

}

void *AndroidWindow::getWindowHandler() {
    return NativeWindow::getWindowHandler();
}
