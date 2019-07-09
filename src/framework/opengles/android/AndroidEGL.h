//
// Created by yongge on 19-7-8.
//

#ifndef ANDROID_ANDROIDEGL_H
#define ANDROID_ANDROIDEGL_H


#include "EGL.h"

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

# include <EGL/egl.h>
# include <GLES3/gl3.h>

#ifdef __cplusplus
}
#endif


namespace freee {


    class AndroidEGL : public EGL {


    public:

        AndroidEGL(int major, int minor);

        AndroidEGL(EGL *shareContext);

        ~AndroidEGL();

        GLContext getContext() override;

        void getVersion(int *major, int *minor) override;

        GLSurface createOffscreenSurface(int w, int h) override;

        GLSurface createWindowSurface(void *nativeWindow) override;

        void destroySurface(GLSurface surface) override;

        void makeCurrent(GLSurface surface) override;

        void swapBuffers(GLSurface surface) override;


    protected:

        int createContext(GLContext shareContext) override;

        void destroyContext() override;

        void initialize() override;

        void terminate() override;


    private:

        EGLConfig   mEGLConfig;
        EGLDisplay  mEGLDisplay;
        EGLContext  mEGLContext;
        std::vector<EGLSurface> mSurfaceList;

    };


}


#endif //ANDROID_ANDROIDEGL_H
