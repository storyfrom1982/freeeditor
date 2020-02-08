//
// Created by yongge on 19-7-12.
//

#include "GLRenderer.h"

#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>


#include <sr_library.h>
#include <sr_malloc.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>




struct gl_window_t {
    ANativeWindow *nativeWindow;
};


gl_window_t* gl_window_create(void *environment, void *jsurface)
{
    LOGD("gl_window_create enter\n");
    gl_window_t *window = (gl_window_t *)calloc(1, sizeof(gl_window_t));
    window->nativeWindow = ANativeWindow_fromSurface((JNIEnv *)environment, (jobject)jsurface);
    if (!window->nativeWindow){
        LOGD("ANativeWindow_fromSurface failed\n");
    }
    LOGD("gl_window_create exit\n");
    return window;
}

void gl_window_release(gl_window_t **pp_window)
{
    LOGD("gl_window_release enter\n");
    if (pp_window && *pp_window){
        gl_window_t *window = *pp_window;
        *pp_window = NULL;
        ANativeWindow_release(window->nativeWindow);
        free(window);
    }
    LOGD("gl_window_release exit\n");
}

void* gl_window_get_native_window(gl_window_t *window)
{
    return window->nativeWindow;
}

int gl_window_get_window_width(gl_window_t *window)
{
    return ANativeWindow_getWidth(window->nativeWindow);
}

int gl_window_get_window_height(gl_window_t *window)
{
    return ANativeWindow_getHeight(window->nativeWindow);
}



struct gl_renderer_t{
    EGLConfig mEGLConfig;
    EGLDisplay mEGLDisplay;
    EGLContext mEGLContext;
    EGLSurface mEGLSurface;
    EGLSurface mEGLOffscreenSurface;
    EGLSurface current_surface;
    void *window;
    int width, height;
};


gl_renderer_t* gl_renderer_create(int w, int h)
{
    LOGD("gl_renderer_create enter\n");

    gl_renderer_t *renderer = (gl_renderer_t*)malloc(sizeof(struct gl_renderer_t));

    renderer->width = w;
    renderer->height = h;

    EGLint major = 1, minor = 4;

    EGLint num_config = 0;

    renderer->mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    LOGD("EGL_DEFAULT_DISPLAY: %p\n", renderer->mEGLDisplay);

    if ( EGL_TRUE != eglInitialize(renderer->mEGLDisplay, &major, &minor)) {
        LOGE("eglInitialize failed\n");
        return NULL;
    }

    const EGLint config_attrib_list[] = {
            EGL_ALPHA_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    if (EGL_TRUE != eglChooseConfig(renderer->mEGLDisplay, config_attrib_list,
                                    &renderer->mEGLConfig, 1, &num_config)){
        LOGE("eglChooseConfig failed\n");
        return NULL;
    }

    EGLint native_visual_id = 0;
    eglGetConfigAttrib(renderer->mEGLDisplay, renderer->mEGLConfig,
                       EGL_NATIVE_VISUAL_ID, &native_visual_id);
    LOGD("eglGetConfigAttrib EGL_NATIVE_VISUAL_ID: %d\n", native_visual_id);

    int context_client_version = 2;
    int client_attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION, context_client_version,
            EGL_NONE
    };

    renderer->mEGLContext = eglCreateContext(renderer->mEGLDisplay,
                                                 renderer->mEGLConfig, EGL_NO_CONTEXT, client_attrib_list);

    if (renderer->mEGLContext == NULL){
        LOGE("eglCreateContext failed\n");
        return NULL;
    }

    const EGLint attrib_list[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
    };

    renderer->mEGLOffscreenSurface = eglCreatePbufferSurface(renderer->mEGLDisplay, renderer->mEGLConfig,
            attrib_list);
    if (renderer->mEGLOffscreenSurface == NULL){
        LOGE("eglCreatePbufferSurface failed %d\n", eglGetError());
        return NULL;
    }

    renderer->current_surface = renderer->mEGLOffscreenSurface;

    if (!eglMakeCurrent(renderer->mEGLDisplay, renderer->current_surface,
                        renderer->current_surface, renderer->mEGLContext)) {
        LOGE("eglMakeCurrent failed\n");
        return NULL;
    }


    LOGD("EGL_VENDOR: %s\n", eglQueryString(renderer->mEGLDisplay, EGL_VENDOR));
    LOGD("EGL_VERSION: %s\n", eglQueryString(renderer->mEGLDisplay, EGL_VERSION));
    LOGD("EGL_CLIENT_APIS: %s\n", eglQueryString(renderer->mEGLDisplay, EGL_CLIENT_APIS));

    LOGD("gl_renderer_create exit\n");

    return renderer;
}

void gl_renderer_release(gl_renderer_t **pp_renderer)
{
    LOGD("gl_renderer_release enter\n");

    if (pp_renderer && *pp_renderer){
        gl_renderer_t *renderer = *pp_renderer;
        *pp_renderer = NULL;

        eglMakeCurrent(renderer->mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (renderer->mEGLSurface){
            eglDestroySurface(renderer->mEGLDisplay, renderer->mEGLSurface);
            renderer->mEGLSurface = NULL;
        }

        if (renderer->mEGLOffscreenSurface){
            eglDestroySurface(renderer->mEGLDisplay, renderer->mEGLOffscreenSurface);
            renderer->mEGLOffscreenSurface = NULL;
        }

        if (renderer->mEGLContext){
            eglDestroyContext(renderer->mEGLDisplay, renderer->mEGLContext);
            renderer->mEGLContext = NULL;
        }

        if (renderer->mEGLDisplay){
            eglTerminate(renderer->mEGLDisplay);
            renderer->mEGLDisplay = NULL;
        }

        free(renderer);
    }

    LOGD("gl_renderer_release exit\n");
}

int gl_renderer_set_window(gl_renderer_t *renderer, void *window)
{
    LOGD("gl_renderer_set_view enter\n");

    if (window == NULL){
        LOGE("Invalid native window\n");
        return -1;
    }

    renderer->window = window;

    renderer->mEGLSurface = eglCreateWindowSurface(renderer->mEGLDisplay,
            renderer->mEGLConfig, (EGLNativeWindowType)window, NULL);
    if (renderer->mEGLSurface == NULL){
        LOGE("eglCreateWindowSurface failed %d\n", eglGetError());
        return -1;
    }

    renderer->current_surface = renderer->mEGLSurface;
    if (!eglMakeCurrent(renderer->mEGLDisplay, renderer->current_surface,
            renderer->current_surface, renderer->mEGLContext)) {
        LOGE("eglMakeCurrent failed\n");
        return -1;
    }

    LOGD("gl_renderer_set_view exit\n");

    return 0;
}

int gl_renderer_remove_window(gl_renderer_t *renderer)
{
    LOGD("gl_renderer_remove_window enter\n");

    renderer->window = NULL;

    if (renderer->mEGLSurface){
        eglDestroySurface(renderer->mEGLDisplay, renderer->mEGLSurface);
        renderer->mEGLSurface = NULL;
    }

    renderer->current_surface = renderer->mEGLOffscreenSurface;
    if (!eglMakeCurrent(renderer->mEGLDisplay, renderer->current_surface,
                        renderer->current_surface, renderer->mEGLContext)) {
        LOGE("eglMakeCurrent failed\n");
        return -1;
    }

    LOGD("gl_renderer_remove_window exit\n");

    return 0;
}

int gl_renderer_swap_buffers(gl_renderer_t *renderer)
{
    if (!eglSwapBuffers(renderer->mEGLDisplay, renderer->current_surface)) {
        LOGE("eglSwapBuffers failed %d\n", eglGetError());
        return -1;
    }

    return 0;
}

int gl_renderer_get_view_width(gl_renderer_t *renderer)
{
    if (renderer->current_surface == renderer->mEGLSurface && renderer->window){
        return gl_window_get_window_width(renderer->window);
    }
    return renderer->width;
}

int gl_renderer_get_view_height(gl_renderer_t *renderer)
{
    if (renderer->current_surface == renderer->mEGLSurface && renderer->window){
        return gl_window_get_window_height(renderer->window);
    }
    return renderer->height;
}