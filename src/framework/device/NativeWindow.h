//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


#ifdef __ANDROID__

#endif


namespace freee {

    class NativeWindow {

    public:

        NativeWindow(void *p);

        virtual ~NativeWindow();

        void* getWindowHandler();

        void getWindowSize(int *w, int *h);

    private:

        void *window;
        void *windowHandler;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
