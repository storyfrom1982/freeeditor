//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


namespace freee {

    class NativeWindow {

    public:

        static NativeWindow* createNativeWindow(void *p);

        virtual ~NativeWindow(){};

        virtual void* getWindowHandler() = 0;

        virtual void getWindowSize(int *w, int *h) = 0;

    };

}


#endif //ANDROID_NATIVEWINDOW_H
