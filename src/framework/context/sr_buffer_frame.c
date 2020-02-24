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
//    frame->color_space = fourcc;
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
//    switch (frame->color_space){
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

int sr_buffer_frame_set_color_space(sr_buffer_frame_t *frame, const uint8_t *data, int width, int height, uint32_t fourcc)
{
    if (frame == NULL || data == NULL){
        return -1;
    }

    frame->color_space = fourcc;
    frame->width = width;
    frame->height = height;

    frame->plane[0].stride = frame->width;
    frame->plane[0].size = (size_t)(frame->width * frame->height);

    frame->data = (uint8_t*)data;
    frame->plane[0].data = frame->data;

    switch (frame->color_space){
        case FOURCC_I420:
            frame->max_plane = 3;
            frame->plane[1].stride = (frame->plane[0].stride + 1) >> 1;
            frame->plane[1].size = frame->plane[0].size >> 2;
            frame->plane[1].data = frame->plane[0].data + frame->plane[0].size;
            frame->plane[2].stride = frame->plane[1].stride;
            frame->plane[2].size = frame->plane[1].size;
            frame->plane[2].data = frame->plane[1].data + frame->plane[1].size;
            break;
        case FOURCC_NV12:
        case FOURCC_NV21:
            frame->max_plane = 2;
            frame->plane[1].stride = frame->plane[0].stride;
            frame->plane[1].size = frame->plane[0].size >> 1;
            frame->plane[1].data = frame->plane[0].data + frame->plane[0].size;
            break;
        default:
            return -1;
    }

    frame->size = 0;

    for (int i = 0; i < frame->max_plane; ++i){
        frame->size += frame->plane[i].size;
    }

    return 0;
}

int sr_buffer_frame_convert_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int _rotation)
{
    if (src == NULL || dst == NULL
        || (src->color_space != FOURCC_NV12
            && src->color_space != FOURCC_NV21)){
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

    int crop_x = (src->width - width + 1) >> 1;
    int crop_y = (src->height - height + 1) >> 1;

    return ConvertToI420(
            src->data, src->size,
            dst->plane[0].data, dst->plane[0].stride,
            dst->plane[1].data, dst->plane[1].stride,
            dst->plane[2].data, dst->plane[2].stride,
            crop_x, crop_y,
            src->width, src->height,
            width, height,
            rotation, src->color_space);
}

int sr_buffer_frame_convert_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst)
{
    if (src->color_space != FOURCC_I420){
        return -1;
    }

    if (dst->color_space == FOURCC_NV21){
        return I420ToNV21(
                src->plane[0].data, src->plane[0].stride,
                src->plane[1].data, src->plane[1].stride,
                src->plane[2].data, src->plane[2].stride,
                dst->plane[0].data, dst->plane[0].stride,
                dst->plane[1].data, dst->plane[1].stride,
                dst->width, dst->height);
    }else if (dst->color_space == FOURCC_NV12){
        return I420ToNV12(
                src->plane[0].data, src->plane[0].stride,
                src->plane[1].data, src->plane[1].stride,
                src->plane[2].data, src->plane[2].stride,
                dst->plane[0].data, dst->plane[0].stride,
                dst->plane[1].data, dst->plane[1].stride,
                dst->width, dst->height);
    }

    return -1;
}
