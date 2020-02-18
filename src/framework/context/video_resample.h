//
// Created by yongge on 19-7-2.
//

#ifndef PROJECT_VIDEO_RESAMPLE_H
#define PROJECT_VIDEO_RESAMPLE_H


#include <libyuv.h>


typedef struct sr_buffer_frame_t {

    int key;

    char *js;

    int is_pointer;

    union {
        void *ptr;
        double f64;
        int64_t i64;
        uint64_t u64;
    };

    struct {
        size_t size;
        uint32_t stride;
        uint8_t *data;
    }plane[4];

    int media_type;

    int max_plane;

    int width, height;

    size_t size;

    uint8_t *data;

}sr_buffer_frame_t;


sr_buffer_frame_t* sr_buffer_frame_alloc(uint32_t width, uint32_t height, uint32_t fourcc);
void sr_buffer_frame_free(sr_buffer_frame_t **pp_frame);

int sr_buffer_frame_fill(sr_buffer_frame_t *frame, const uint8_t *data, uint32_t width, uint32_t height, uint32_t fourcc);
int sr_buffer_frame_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int rotate_degree);
int sr_buffer_frame_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst);


#endif //PROJECT_VIDEO_RESAMPLE_H
