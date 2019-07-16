//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_ANDROIDWINDOW_H
#define ANDROID_ANDROIDWINDOW_H

#include "../NativeWindow.h"

namespace freee {

    class Window : public NativeWindow {

    public:

        Window(void *windowHandler);
        virtual ~Window();

        virtual void* getWindowHandler() override ;

        void getWindowSize(int *w, int *h) override;


    private:

        void *mWindowHandler;

    };
}


#endif //ANDROID_ANDROIDWINDOW_H
