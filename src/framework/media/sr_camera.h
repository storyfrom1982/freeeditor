//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_SR_CAMERA_H
#define ANDROID_SR_CAMERA_H


#define CAMERA_ID_BACK      0
#define CAMERA_ID_FRONT      1

#define CAMERA_YUV_I420      0
#define CAMERA_YUV_NV21      1
#define CAMERA_YUV_NV12      3

typedef struct sr__camera__{
    int (*open_camera)(struct sr__camera__*, int width, int height, int fps, int yuv, int camera_id);
    void (*close_camera)(struct sr__camera__*);
    int (*start_capture)(struct sr__camera__*);
    void (*stop_capture)(struct sr__camera__*);
    void (*change_id)(struct sr__camera__*, int camera_id);
    void (*change_size)(struct sr__camera__*, int width, int height);
    void (*change_fps)(struct sr__camera__*, int fps);
    int (*get_fps)(struct sr__camera__*);
    int (*get_width)(struct sr__camera__*);
    int (*get_height)(struct sr__camera__*);
    int (*get_id)(struct sr__camera__*);
    int (*get_error)(struct sr__camera__*);
    void (*set_picture_callback)(struct sr__camera__*, void *handler, int (*process_data)(void *, void *data, int size));
    void (*set_event_callback)(struct sr__camera__*, void *handler, int (*process_event)(void *, int event));
    struct __camera__t *camera;
}sr__camera__t;

sr__camera__t* sr__camera__create();
void sr__camera__release(sr__camera__t **pp_camera);




#endif //ANDROID_SR_CAMERA_H
