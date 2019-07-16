//
// Created by yongge on 19-7-11.
//

#ifndef ANDROID_VIDEORENDERER_H
#define ANDROID_VIDEORENDERER_H

#include <IMsgListener.h>
#include <EGL.h>
#include <GLObject.h>
#include <NativeWindow.h>

namespace freee {


    class VideoRenderer : public IMsgListener{

    public:

        VideoRenderer(int w, int h);
        ~VideoRenderer();

        void setNativeWindow(NativeWindow *window);
        void addElement(GLObject *obj);


    private:

        void msgSetNativeWindow(sr_msg_t msg);
        void msgAddElement(sr_msg_t msg);

        void messageProcessorLoop();
        static void* messageProcessorThread(void *p);


    private:

        int width;
        int height;

        EGL *egl;
        GLSurface offscreen;
        GLSurface viewSurface;
        GLSurface mainSurface;
        NativeWindow *window;

        std::vector<GLObject*> objList;

        pthread_t tid;
        sr_msg_queue_t *queue;

        glm::mat4 mProjectMat;
        glm::mat4 mViewMat;
    };

}



#endif //ANDROID_VIDEORENDERER_H
