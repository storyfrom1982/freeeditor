//
// Created by yongge on 19-7-4.
//

#ifndef ANDROID_ANDROIDWINDOW_H
#define ANDROID_ANDROIDWINDOW_H

#include "../NativeWindow.h"


#ifdef __cplusplus
extern "C" {
#endif

# include <GLRenderer.h>

#ifdef __cplusplus
}
#endif

namespace freee {

    class Window{

    public:

        Window();
        virtual ~Window();

        virtual void* getWindowHandler();

        void getWindowSize(int *w, int *h);


    private:

        gl_window_t *window;
        gl_renderer_t *renderer;

    };
}


#endif //ANDROID_ANDROIDWINDOW_H
