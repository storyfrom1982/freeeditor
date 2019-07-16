//
// Created by yongge on 19-7-12.
//

#ifndef ANDROID_EGL_INTERFACE_H
#define ANDROID_EGL_INTERFACE_H



typedef struct gl_window_t gl_window_t;


gl_window_t* gl_window_create(void *environment, void *native_window);
void gl_window_release(gl_window_t **pp_window);
void* gl_window_get_native_window(gl_window_t *window);
int gl_window_get_window_width(gl_window_t *window);
int gl_window_get_window_height(gl_window_t *window);


typedef struct gl_renderer_t gl_renderer_t;


gl_renderer_t* gl_renderer_create(int w, int h);
void gl_renderer_release(gl_renderer_t **pp_renderer);
int gl_renderer_set_window(gl_renderer_t *renderer, gl_window_t *window);
int gl_renderer_remove_window(gl_renderer_t *renderer);
int gl_renderer_swap_buffers(gl_renderer_t *renderer);
int gl_renderer_get_view_width(gl_renderer_t *renderer);
int gl_renderer_get_view_height(gl_renderer_t *renderer);


#endif //ANDROID_EGL_INTERFACE_H
