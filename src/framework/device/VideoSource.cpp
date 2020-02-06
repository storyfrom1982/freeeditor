//
// Created by yongge on 19-6-16.
//

#include "VideoSource.h"

#ifdef __ANDROID__
#include <Camera.h>
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <libyuv.h>

//#ifdef __cplusplus
//}
//#endif


using namespace freee;

freee::VideoSource *freee::VideoSource::openVideoSource(DeviceContext *ctx) {

    VideoSource *camera = NULL;

#ifdef __ANDROID__
    camera = new Camera(ctx);
#endif

    return camera;
}

VideoSource::VideoSource(DeviceContext *ctx) : DeviceContext(ctx) {
}

VideoSource::~VideoSource() {
//    m_listener->removeOutputStream(this);
    imageFilter(NULL, 0, 0, 0 , NULL, 0, 0, 0, 0);
}

int VideoSource::imageFilter(void *src, int src_w, int src_h, int src_fmt, void *dst, int dst_w,
                             int dst_h, int dst_fmt, int rotate) {

    uint8_t *nv21 = (uint8_t *) src;
    size_t size = 0;
    uint8_t *yuv420 = (uint8_t *) dst;

    int src_width, src_height;
    int dst_width, dst_height;
    int dst_y_size;

    enum libyuv::RotationMode rotation;

    src_width = src_w;
    src_height = src_h;

    if (rotate == 90){
        dst_width = dst_h;
        dst_height = dst_w;
        rotation = libyuv::kRotate90;
    }else if (rotate == 180){
        dst_width = dst_w;
        dst_height = dst_h;
        rotation = libyuv::kRotate180;
    } else if (rotate == 270){
        dst_width = dst_h;
        dst_height = dst_w;
        rotation = libyuv::kRotate270;
    }else{
        dst_width = dst_w;
        dst_height = dst_h;
        rotation = libyuv::kRotate0;
    }

    dst_y_size = dst_width * dst_height;


    uint8_t* dst_y = yuv420;
    int dst_stride_y = dst_width;

    uint8_t* dst_u = dst_y + dst_y_size;
    int dst_stride_u = dst_width >> 1;

    uint8_t* dst_v = dst_u + (dst_y_size >> 2);
    int dst_stride_v = dst_width >> 1;

    int crop_x = (src_w - dst_w) >> 1;
    int crop_y = (src_h - dst_h) >> 1;

    libyuv::ConvertToI420(nv21, size,
                          dst_y, dst_stride_y,
                          dst_u, dst_stride_u,
                          dst_v, dst_stride_v,
                          crop_x, crop_y,
                          src_width, src_height,
                          dst_width, dst_height,
                          rotation, libyuv::FOURCC_NV21);

    libyuv::I420ToNV21(
            dst_y, dst_stride_y,
            dst_u, dst_stride_u,
            dst_v, dst_stride_v,
            nv21, dst_width,
            nv21 + dst_y_size, dst_width,
            dst_width, dst_height);

    return 0;
}

void VideoSource::setEncoder(DeviceContext *ctx) {
    outputCtx = ctx;
}

void VideoSource::processData(void *data, int size) {
    outputCtx->onPutData(data, size);
}

