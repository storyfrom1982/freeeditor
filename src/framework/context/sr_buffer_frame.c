//
// Created by yongge on 20-2-5.
//

#include "sr_buffer_frame.h"


#include <libyuv.h>


//sr_buffer_frame_t* sr_buffer_frame_alloc(uint32_t width, uint32_t height, uint32_t fourcc)
//{
//    if (fourcc != FOURCC_I420
//        && fourcc != FOURCC_NV21
//        && fourcc != FOURCC_NV12){
//        return NULL;
//    }
//
//    sr_buffer_frame_t *frame = (sr_buffer_frame_t *)malloc(sizeof(sr_buffer_frame_t));
//
//    frame->is_pointer = 1;
//
//    frame->image_format = fourcc;
//    frame->width = width;
//    frame->height = height;
//
//    frame->plane[0].stride = frame->width;
//    frame->plane[0].size = (size_t)(frame->width * frame->height);
//    frame->size = frame->plane[0].size + (frame->plane[0].size >> 1);
//
//    frame->data = (uint8_t *)malloc(frame->size);
//    frame->plane[0].data = frame->data;
//
//    switch (frame->image_format){
//        case FOURCC_I420:
//            frame->max_plane = 3;
//            frame->plane[1].stride = frame->plane[0].stride >> 1;
//            frame->plane[1].size = frame->plane[0].size >> 2;
//            frame->plane[1].data = frame->plane[0].data + frame->plane[0].size;
//            frame->plane[2].stride = frame->plane[1].stride;
//            frame->plane[2].size = frame->plane[1].size;
//            frame->plane[2].data = frame->plane[1].data + frame->plane[1].size;
//            break;
//        case FOURCC_NV12:
//        case FOURCC_NV21:
//            frame->max_plane = 2;
//            frame->plane[1].stride = frame->plane[0].stride;
//            frame->plane[1].size = frame->plane[0].size >> 1;
//            frame->plane[1].data = frame->plane[0].data + frame->plane[0].size;
//            break;
//        default:
//            break;
//    }
//
//    frame->size = 0;
//
//    return frame;
//}
//
//void sr_buffer_frame_free(sr_buffer_frame_t **pp_frame)
//{
//    if (pp_frame != NULL && *pp_frame != NULL){
//        sr_buffer_frame_t *packet = *pp_frame;
//        *pp_frame = NULL;
//        if (packet->is_pointer){
//            if (packet->data){
//                free(packet->data);
//            }
//            free(packet);
//        }
//    }
//}

int sr_buffer_frame_set_image_format(sr_buffer_frame_t *frame, const uint8_t *data, int width,
                                     int height, uint32_t fourcc)
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

int sr_buffer_frame_convert_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int _rotation)
{
    if (src == NULL || dst == NULL
        || (src->video_format != FOURCC_NV12
            && src->video_format != FOURCC_NV21)){
        return -1;
    }

    int width = dst->width;
    int height = dst->height;
    enum RotationMode rotation = kRotate0;

    if (_rotation == 90){
        width = dst->height;
        height = dst->width;
        rotation = kRotate90;
    }else if (_rotation == 270){
        width = dst->height;
        height = dst->width;
        rotation = kRotate270;
    }else if (_rotation == 180){
        rotation = kRotate180;
    }

    src->x = (src->width - width + 1) >> 1;
    src->y = (src->height - height + 1) >> 1;

    return ConvertToI420(
            src->data, src->size,
            dst->channel[0].data, dst->channel[0].stride,
            dst->channel[1].data, dst->channel[1].stride,
            dst->channel[2].data, dst->channel[2].stride,
            src->x, src->y,
            src->width, src->height,
            width, height,
            rotation, src->video_format);
}

int sr_buffer_frame_convert_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst)
{
    if (src->video_format != FOURCC_I420){
        return -1;
    }

    if (dst->video_format == FOURCC_NV21){
        return I420ToNV21(
                src->channel[0].data, src->channel[0].stride,
                src->channel[1].data, src->channel[1].stride,
                src->channel[2].data, src->channel[2].stride,
                dst->channel[0].data, dst->channel[0].stride,
                dst->channel[1].data, dst->channel[1].stride,
                dst->width, dst->height);
    }else if (dst->video_format == FOURCC_NV12){
        return I420ToNV12(
                src->channel[0].data, src->channel[0].stride,
                src->channel[1].data, src->channel[1].stride,
                src->channel[2].data, src->channel[2].stride,
                dst->channel[0].data, dst->channel[0].stride,
                dst->channel[1].data, dst->channel[1].stride,
                dst->width, dst->height);
    }

    return -1;
}
