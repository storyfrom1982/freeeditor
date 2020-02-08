//
// Created by yongge on 19-7-2.
//

#ifndef PROJECT_VIDEO_RESAMPLE_H
#define PROJECT_VIDEO_RESAMPLE_H


#include <libyuv.h>


typedef struct VideoPacket {

    uint32_t type;
    int is_malloc;
    int nb_planes;

    struct {
        int stride;
        size_t size;
        uint8_t *data;
    }plane[4];

    int width;
    int height;
    size_t size;
    uint8_t *data;
}VideoPacket;


VideoPacket* videoPacket_Alloc(int width, int height, uint32_t fourcc);

int videoPacket_FillData(VideoPacket *packet, const uint8_t *data, int width, int height, uint32_t fourcc);
int videoPacket_To_YUV420(VideoPacket *src, VideoPacket *dst, int rotate_degree);
int videoPacket_From_YUV420(VideoPacket *src, VideoPacket *dst);
void videoPacket_Free(VideoPacket **pp_packet);

#endif //PROJECT_VIDEO_RESAMPLE_H
