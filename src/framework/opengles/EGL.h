//
// Created by yongge on 19-7-8.
//

#ifndef ANDROID_EGLCONTEXT_H
#define ANDROID_EGLCONTEXT_H


namespace freee {


    typedef void* GLSurface;
    typedef void* GLContext;


    class EGL {

    public:

        EGL (int major, int minor);

        EGL (EGL *shareContext);

        virtual ~EGL();

        static EGL* createEglContext(EGL *shareContext);

        static EGL* createEglContext(int major, int minor);

        virtual GLSurface createOffscreenSurface(int w, int h) = 0;

        virtual GLSurface createWindowSurface(void *nativeWindow) = 0;

        virtual void destroySurface(GLSurface surface) = 0;

        virtual void makeCurrent(GLSurface surface) = 0;

        virtual void swapBuffers(GLSurface surface) = 0;

        virtual GLContext getContext() = 0;

        virtual void getVersion(int *major, int *minor) = 0;


    protected:


        virtual int createContext(GLContext shareContext) = 0;

        virtual void destroyContext() = 0;

        virtual void initialize() = 0;

        virtual void terminate() = 0;


    protected:

        int mMajor, mMinor;

    };


}



#endif //ANDROID_EGLCONTEXT_H
