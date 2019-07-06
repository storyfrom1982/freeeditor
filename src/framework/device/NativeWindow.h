//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_NATIVEWINDOW_H
#define ANDROID_NATIVEWINDOW_H


namespace freee {

    class NativeWindow {

    public:

        static NativeWindow* createNativeWindow();

        virtual ~NativeWindow(){};

        virtual void* getWindowHandler();

    };

}


#endif //ANDROID_NATIVEWINDOW_H
