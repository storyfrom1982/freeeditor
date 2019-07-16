//
// Created by yongge on 19-7-8.
//

#include "EGL.h"

#ifdef __ANDROID__
# include "AndroidEGL.h"
#endif

using namespace freee;


EGL::EGL(EGL *shareContext) {
    mMajor = 3;
    mMinor = 0;
}

EGL::EGL(int major, int minor)
    : mMajor(major), mMinor(minor) {

}

EGL::~EGL() {

}

EGL *EGL::createEglContext(EGL *shareContext) {
    EGL *egl = NULL;
#ifdef __ANDROID__
    egl = new AndroidEGL(shareContext);
#endif
    return egl;
}

EGL *EGL::createEglContext(int major, int minor) {
    EGL *egl = NULL;
#ifdef __ANDROID__
    egl = new AndroidEGL(major, minor);
#endif
    return egl;
}
