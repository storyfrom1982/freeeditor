//
// Created by yongge on 19-7-11.
//

#include "VideoRenderer.h"


#define  MAX_UPDATE_FPS (1000000/60)
#define  EYE_Z_POS      0.0f
#define  NEAR_Z_POS     1.0f
#define  VIDEO_Z_POS    100.0f
#define  FAR_Z_POS      (100.0f*2)

using namespace freee;

VideoRenderer::VideoRenderer(int w, int h) {
    width = w;
    height = h;
    queue = sr_msg_queue_create();
    pthread_create(&tid, NULL, VideoRenderer::messageProcessorThread, this);
}

VideoRenderer::~VideoRenderer() {

}

void VideoRenderer::setNativeWindow(NativeWindow *window) {
    sr_msg_t msg = {0};
    msg.type = 0;
    msg.ptr = window;
    sr_msg_queue_push(queue, msg);
}

void VideoRenderer::addElement(GLObject *obj) {
    sr_msg_t msg = {0};
    msg.type = 1;
    msg.ptr = obj;
    sr_msg_queue_push(queue, msg);
}

void VideoRenderer::messageProcessorLoop() {

    egl = EGL::createEglContext(NULL);

    int major = 0;
    int minor = 0;
    egl->getVersion(&major, &minor);

    LOGD("gles version %d.%d\n", major, minor);

    mainSurface = offscreen = egl->createOffscreenSurface(width, height);

    sr_msg_t msg = {0};

    bool running = true;

    while (running){

        if (sr_msg_queue_popable(queue)){
            if (sr_msg_queue_pop(queue, &msg) < 0){
                break;
            }

            switch (msg.type){
                case 0:
                    msgSetNativeWindow(msg);
                    break;
                case 1:
                    msgAddElement(msg);
                    break;
                default:
                    break;
            }
        }

        egl->makeCurrent(mainSurface);

        glClearColor(1.0f, 1.0f, 0, 0.0f);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!objList.empty()){
            for (int i = 0; i < objList.size(); ++i){
                LOGD("draw===============================\n");
                objList[i]->draw(mProjectMat, mViewMat);
            }
        }

        glDisable(GL_BLEND);

        egl->swapBuffers(mainSurface);
    }

}

void* VideoRenderer::messageProcessorThread(void *p) {
    static_cast<VideoRenderer *>(p)->messageProcessorLoop();
    return NULL;
}

void VideoRenderer::msgSetNativeWindow(sr_msg_t msg) {
    if (viewSurface){
        egl->destroySurface(viewSurface);
    }
    NativeWindow *nativeWindow = (NativeWindow *)msg.ptr;
    nativeWindow->getWindowSize(&width, &height);
    mainSurface = viewSurface = egl->createWindowSurface(nativeWindow->getWindowHandler());

    mViewMat = {
            {1,    0,    0.0,     0.0},
            {0,    1,    0.0,     0.0},
            {0.0,  0.0,  -1,     0.000},
            {0.0,  0.0,  -VIDEO_Z_POS, 1.0},
    };

    float w = width;
    float h = height;
    float nearVal = VIDEO_Z_POS;
    float farVal = FAR_Z_POS;

    float  left  = -w/2;
    float  right = w/2;

    float  top  =   h/2;
    float  bottom = -h/2;

    float A =  (right+left)/(right-left);
    float B =(top+bottom)/(top-bottom);
    float C= -(farVal+nearVal) /( farVal-nearVal );
    float D= -2*(farVal*nearVal) /( farVal-nearVal );

    // calculate the project matrix
    mProjectMat = {
            {2*nearVal/(right-left), 0,                     A,   0.0},
            {0,                      2*nearVal/(top-bottom), B,  0.0},
            {0.0,                    0.0,                    C,   D },
            {0.0,                       0.0,                 -1.0, 0},
    };

    glViewport(0, 0, width, height);
}

void VideoRenderer::msgAddElement(sr_msg_t msg) {
    GLObject *obj = (GLObject *)msg.ptr;
    objList.push_back(obj);
    obj->build();
}
