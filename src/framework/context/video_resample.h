//
// Created by yongge on 19-7-2.
//

#ifndef PROJECT_VIDEO_RESAMPLE_H
#define PROJECT_VIDEO_RESAMPLE_H


#include <libyuv.h>


typedef struct sr_buffer_frame_t {

    int media_type;

    int max_plane;

    int width, height;

    int64_t timestamp;

    struct {
        size_t size;
        uint32_t stride;
        uint8_t *data;
    }plane[4];

    size_t size;

    uint8_t *data;

}sr_buffer_frame_t;

int sr_buffer_frame_fill_picture(sr_buffer_frame_t *frame, const uint8_t *data, uint32_t width, uint32_t height, uint32_t fourcc);
int sr_buffer_frame_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int rotate_degree);
int sr_buffer_frame_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst);


#endif //PROJECT_VIDEO_RESAMPLE_H
