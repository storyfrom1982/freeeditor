//
// Created by yongge on 19-7-8.
//

#include "AndroidEGL.h"


#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>

#ifdef __cplusplus
}
#endif


using namespace freee;


AndroidEGL::AndroidEGL(int major, int minor) : EGL(major, minor)
{
    initialize();
    createContext(NULL);
}

AndroidEGL::AndroidEGL(EGL *shareContext) : EGL(shareContext)
{
    GLContext context = NULL;
    if (shareContext){
        context = shareContext->getContext();
        shareContext->getVersion(&mMajor, &mMinor);
    }
    initialize();
    createContext(context);
}

AndroidEGL::~AndroidEGL() {
    destroyContext();
    terminate();
}


GLContext AndroidEGL::getContext() {
    return mEGLContext;
}


void AndroidEGL::getVersion(int *major, int *minor) {
    if (major && minor){
        *major = mMajor;
        *minor = mMinor;
    }
}


GLSurface AndroidEGL::createOffscreenSurface(int w, int h) {
    LOGD("AndroidEGL::createOffscreenSurface enter\n");
    const EGLint attrib_list[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
    };
    EGLSurface eglSurface = eglCreatePbufferSurface(mEGLDisplay, mEGLConfig, attrib_list);
    if ( eglSurface == NULL){
        LOGE("eglCreatePbufferSurface failed %d\n", eglGetError());
        return NULL;
    }
    mSurfaceList.push_back(eglSurface);
    LOGD("AndroidEGL::createOffscreenSurface exit\n");
    return (GLSurface)eglSurface;
}


GLSurface AndroidEGL::createWindowSurface(void *nativeWindow) {
    LOGD("AndroidEGL::createWindowSurface enter\n");
    if (nativeWindow == NULL){
        LOGE("Invalid native window\n");
        return NULL;
    }
    EGLSurface eglSurface = eglCreateWindowSurface(mEGLDisplay, mEGLConfig, (EGLNativeWindowType)nativeWindow, NULL);
    if ( eglSurface == NULL){
        LOGE("eglCreateWindowSurface failed %d\n", eglGetError());
        return NULL;
    }
    mSurfaceList.push_back(eglSurface);
    LOGD("AndroidEGL::createWindowSurface exit\n");
    return (GLSurface)eglSurface;
}


void AndroidEGL::destroySurface(GLSurface surface) {
    LOGD("AndroidEGL::destroySurface enter\n");
    if (!mSurfaceList.empty()){
        for (int i = 0; i < mSurfaceList.size(); ++i){
            if (mSurfaceList[i] == (EGLSurface)surface){
                eglDestroySurface(mEGLDisplay, mSurfaceList[i]);
                mSurfaceList.erase(mSurfaceList.begin() + i);
                break;
            }
        }
    }
    LOGD("AndroidEGL::destroySurface exit\n");
}


void AndroidEGL::makeCurrent(GLSurface surface) {
    if (!eglMakeCurrent(mEGLDisplay, (EGLSurface)surface, (EGLSurface)surface, mEGLContext))  {
        LOGE("eglMakeCurrent failed\n");
    }
}


void AndroidEGL::swapBuffers(GLSurface surface) {
    if (!eglSwapBuffers(mEGLDisplay, (EGLSurface)surface))  {
        LOGE("eglSwapBuffers failed %d\n", eglGetError());
    }
}


int AndroidEGL::createContext(GLContext shareContext) {

    LOGD("AndroidEGL::createContext exit\n");

    EGLint num_config = 0;
    EGLContext shared_context = EGL_NO_CONTEXT;

    if (shareContext){
        shared_context = (EGLContext)shareContext;
    }

    mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    LOGD("EGL_DEFAULT_DISPLAY: %p\n", mEGLDisplay);

    if ( EGL_TRUE != eglInitialize(mEGLDisplay, &mMajor, &mMinor)) {
        LOGE("eglInitialize failed\n");
        return -1;
    }

    LOGD("eglInitialize version: %d.%d\n", mMajor, mMinor);

    const EGLint config_attrib_list[] = {
            EGL_ALPHA_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    if (EGL_TRUE != eglChooseConfig(mEGLDisplay, config_attrib_list, &mEGLConfig, 1, &num_config)){
        LOGE("eglChooseConfig failed\n");
        return -1;
    }

    EGLint native_visual_id = 0;
    eglGetConfigAttrib(mEGLDisplay, mEGLConfig, EGL_NATIVE_VISUAL_ID, &native_visual_id);
    LOGD("eglChooseConfig EGL_NATIVE_VISUAL_ID: %d\n", native_visual_id);

    int context_client_version = 2;
    int client_attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION, context_client_version,
            EGL_NONE
    };

    mEGLContext = eglCreateContext(mEGLDisplay, mEGLConfig, shared_context, client_attrib_list);

    if ( mEGLContext == NULL){
        LOGE("eglCreateContext failed\n");
        return -1;
    }

    LOGD("EGL_VENDOR: %s\n", eglQueryString(mEGLDisplay, EGL_VENDOR));
    LOGD("EGL_VERSION: %s\n", eglQueryString(mEGLDisplay, EGL_VERSION));
    LOGD("EGL_CLIENT_APIS: %s\n", eglQueryString(mEGLDisplay, EGL_CLIENT_APIS));

    LOGD("AndroidEGL::createContext exit\n");

    return 0;
}


void AndroidEGL::destroyContext() {

    LOGD("AndroidEGL::destroyContext enter\n");

    eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (!mSurfaceList.empty()){
        for (int i = 0; i < mSurfaceList.size(); ++i){
            if (mSurfaceList[i]){
                eglDestroySurface(mEGLDisplay, mSurfaceList[i]);
            }
            mSurfaceList.erase(mSurfaceList.begin() + i);
        }
        mSurfaceList.clear();
    }

    if (mEGLContext){
        eglDestroyContext(mEGLDisplay, mEGLContext);
        mEGLContext = NULL;
    }

    if (mEGLDisplay){
        eglTerminate(mEGLDisplay);
        mEGLDisplay = NULL;
    }

    LOGD("AndroidEGL::destroyContext exit\n");
}


void AndroidEGL::initialize() {

}


void AndroidEGL::terminate() {

}
