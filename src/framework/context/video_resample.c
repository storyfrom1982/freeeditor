//
// Created by yongge on 20-2-5.
//

#include "video_resample.h"


#include <libyuv.h>


VideoPacket* videoPacket_Alloc(int width, int height, uint32_t fourcc)
{
    if (fourcc != FOURCC_I420
        && fourcc != FOURCC_NV21
        && fourcc != FOURCC_NV12){
        return NULL;
    }

    VideoPacket *packet = (VideoPacket *)malloc(sizeof(VideoPacket));

    packet->type = fourcc;
    packet->width = width;
    packet->height = height;

    packet->plane[0].stride = packet->width;
    packet->plane[0].size = (size_t)(packet->width * packet->height);
    packet->size = packet->plane[0].size + (packet->plane[0].size >> 1);

    packet->is_malloc = 1;
    packet->data = (uint8_t *)malloc(packet->size);
    packet->plane[0].data = packet->data;

    switch (packet->type){
        case FOURCC_I420:
            packet->nb_planes = 3;
            packet->plane[1].stride = packet->plane[0].stride >> 1;
            packet->plane[1].size = packet->plane[0].size >> 2;
            packet->plane[1].data = packet->plane[0].data + packet->plane[0].size;
            packet->plane[2].stride = packet->plane[1].stride;
            packet->plane[2].size = packet->plane[1].size;
            packet->plane[2].data = packet->plane[1].data + packet->plane[1].size;
            break;
        case FOURCC_NV12:
        case FOURCC_NV21:
            packet->nb_planes = 2;
            packet->plane[1].stride = packet->plane[0].stride;
            packet->plane[1].size = packet->plane[0].size >> 1;
            packet->plane[1].data = packet->plane[0].data + packet->plane[0].size;
            break;
        default:
            break;
    }

    return packet;
}

void videoPacket_Free(VideoPacket **pp_packet)
{
    if (pp_packet != NULL && *pp_packet != NULL){
        VideoPacket *packet = *pp_packet;
        *pp_packet = NULL;
        if (packet->is_malloc){
            if (packet->data){
                free(packet->data);
            }
            free(packet);
        }
    }
}

int videoPacket_FillData(VideoPacket *packet, const uint8_t *data, int width, int height, uint32_t fourcc)
{
    if (packet == NULL || data == NULL){
        return -1;
    }

    packet->type = fourcc;
    packet->width = width;
    packet->height = height;

    packet->plane[0].stride = packet->width;
    packet->plane[0].size = (size_t)(packet->width * packet->height);
    packet->size = packet->plane[0].size + (packet->plane[0].size >> 1);

    packet->is_malloc = 0;
    packet->data = (uint8_t*)data;
    packet->plane[0].data = packet->data;

    switch (packet->type){
        case FOURCC_I420:
            packet->nb_planes = 3;
            packet->plane[1].stride = packet->plane[0].stride >> 1;
            packet->plane[1].size = packet->plane[0].size >> 2;
            packet->plane[1].data = packet->plane[0].data + packet->plane[0].size;
            packet->plane[2].stride = packet->plane[1].stride;
            packet->plane[2].size = packet->plane[1].size;
            packet->plane[2].data = packet->plane[1].data + packet->plane[1].size;
            break;
        case FOURCC_NV12:
        case FOURCC_NV21:
            packet->nb_planes = 2;
            packet->plane[1].stride = packet->plane[0].stride;
            packet->plane[1].size = packet->plane[0].size >> 1;
            packet->plane[1].data = packet->plane[0].data + packet->plane[0].size;
            break;
        default:
            return -1;
    }

    return 0;
}

int videoPacket_To_YUV420(VideoPacket *src, VideoPacket *dst, int rotate_degree)
{
    if (src == NULL || dst == NULL
        || (src->type != FOURCC_NV12
            && src->type != FOURCC_NV21)){
        return -1;
    }

    int width = dst->width;
    int height = dst->height;
    enum RotationMode rotation = kRotate0;

    if (rotate_degree == 90){
        width = dst->height;
        height = dst->width;
        rotation = kRotate90;
    }else if (rotate_degree == 270){
        width = dst->height;
        height = dst->width;
        rotation = kRotate270;
    }else if (rotate_degree == 180){
        rotation = kRotate180;
    }

    int crop_x = (src->width - width) >> 1;
    int crop_y = (src->height - height) >> 1;

    return ConvertToI420(
            src->data, src->size,
            dst->plane[0].data, dst->plane[0].stride,
            dst->plane[1].data, dst->plane[1].stride,
            dst->plane[2].data, dst->plane[2].stride,
            crop_x, crop_y,
            src->width, src->height,
            width, height,
            rotation, src->type);
}

int videoPacket_From_YUV420(VideoPacket *src, VideoPacket *dst)
{
    if (src->type != FOURCC_I420){
        return -1;
    }

    if (dst->type == FOURCC_NV21){
        return I420ToNV21(
                src->plane[0].data, src->plane[0].stride,
                src->plane[1].data, src->plane[1].stride,
                src->plane[2].data, src->plane[2].stride,
                dst->plane[0].data, dst->plane[0].stride,
                dst->plane[1].data, dst->plane[1].stride,
                dst->width, dst->height);
    }else if (dst->type == FOURCC_NV12){
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
