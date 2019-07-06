//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_ANDROIDWINDOW_H
#define ANDROID_ANDROIDWINDOW_H

#include "../NativeWindow.h"

namespace freee {

    class Window : public NativeWindow {

    public:

        Window();
        virtual ~Window();

        virtual void* getWindowHandler() override ;

    };
}


#endif //ANDROID_ANDROIDWINDOW_H
