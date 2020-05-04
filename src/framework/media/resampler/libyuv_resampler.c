//
// Created by yongge on 20-2-5.
//

#include "libyuv_resampler.h"


#include <libyuv.h>

uint32_t libyuv_convert_fourcc(const char *p_fourcc)
{
    union {
        uint32_t format;
        unsigned char fourcc[4];
    }fourcctoint = {.fourcc[0] = p_fourcc[0],
                    .fourcc[1] = p_fourcc[1],
                    .fourcc[2] = p_fourcc[2],
                    .fourcc[3] = p_fourcc[3]};
    return fourcctoint.format;
}

int libyuv_set_format(sr_msg_frame_t *frame, const uint8_t *data, int width, int height, uint32_t fourcc)
{
    if (frame == NULL || data == NULL){
        return -1;
    }

    frame->video_format = fourcc;
    frame->width = width;
    frame->height = height;

    frame->channel[0].stride = frame->width;
    frame->channel[0].size = (size_t)(frame->width * frame->height);

    frame->data = (uint8_t*)data;
    frame->channel[0].data = frame->data;

    switch (frame->video_format){
        case FOURCC_I420:
            frame->channel_count = 3;
            frame->channel[1].stride = (frame->channel[0].stride + 1) >> 1;
            frame->channel[1].size = frame->channel[0].size >> 2;
            frame->channel[1].data = frame->channel[0].data + frame->channel[0].size;
            frame->channel[2].stride = frame->channel[1].stride;
            frame->channel[2].size = frame->channel[1].size;
            frame->channel[2].data = frame->channel[1].data + frame->channel[1].size;
            break;
        case FOURCC_NV12:
        case FOURCC_NV21:
            frame->channel_count = 2;
            frame->channel[1].stride = frame->channel[0].stride;
            frame->channel[1].size = frame->channel[0].size >> 1;
            frame->channel[1].data = frame->channel[0].data + frame->channel[0].size;
            break;
        default:
            return -1;
    }

    frame->size = 0;

    for (int i = 0; i < frame->channel_count; ++i){
        frame->size += frame->channel[i].size;
    }

    return 0;
}

int libyuv_convert_to_yuv420p(sr_msg_frame_t *src, sr_msg_frame_t *yuv420p, int _rotation)
{
    if (src == NULL || yuv420p == NULL
        || (src->video_format != FOURCC_NV12
            && src->video_format != FOURCC_NV21)){
        return -1;
    }

    int width = yuv420p->width;
    int height = yuv420p->height;
    enum RotationMode rotation = kRotate0;

    if (_rotation == 90){
        width = yuv420p->height;
        height = yuv420p->width;
        rotation = kRotate90;
    }else if (_rotation == 270){
        width = yuv420p->height;
        height = yuv420p->width;
        rotation = kRotate270;
    }else if (_rotation == 180){
        rotation = kRotate180;
    }

    if ((float)src->width / src->height != (float)width / height){
        src->x = ((src->width - width) + 1) >> 1;
        src->y = ((src->height - height) + 1) >> 1;
    }else {
        src->x = src->y = 0;
    }

    return ConvertToI420(
            src->data, src->size,
            yuv420p->channel[0].data, yuv420p->channel[0].stride,
            yuv420p->channel[1].data, yuv420p->channel[1].stride,
            yuv420p->channel[2].data, yuv420p->channel[2].stride,
            src->x, src->y,
            src->width, src->height,
            width, height,
            rotation, src->video_format);
}

int libyuv_convert_from_yuv420p(sr_msg_frame_t *yuv420p, sr_msg_frame_t *dst)
{
    if (yuv420p->video_format != FOURCC_I420){
        return -1;
    }

    if (dst->video_format == FOURCC_NV21){
        return I420ToNV21(
                yuv420p->channel[0].data, yuv420p->channel[0].stride,
                yuv420p->channel[1].data, yuv420p->channel[1].stride,
                yuv420p->channel[2].data, yuv420p->channel[2].stride,
                dst->channel[0].data, dst->channel[0].stride,
                dst->channel[1].data, dst->channel[1].stride,
                dst->width, dst->height);
    }else if (dst->video_format == FOURCC_NV12){
        return I420ToNV12(
                yuv420p->channel[0].data, yuv420p->channel[0].stride,
                yuv420p->channel[1].data, yuv420p->channel[1].stride,
                yuv420p->channel[2].data, yuv420p->channel[2].stride,
                dst->channel[0].data, dst->channel[0].stride,
                dst->channel[1].data, dst->channel[1].stride,
                dst->width, dst->height);
    }

    return -1;
}
