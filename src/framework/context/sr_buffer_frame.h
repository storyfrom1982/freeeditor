//
// Created by yongge on 19-7-2.
//

#ifndef PROJECT_SR_BUFFER_FRAME_H
#define PROJECT_SR_BUFFER_FRAME_H


#include <libyuv.h>


typedef struct sr_buffer_frame_t {

    int media_type;
    int flag;

    union {
        struct {
            int width;
            int height;
            int image_format;
        };
        struct {
            int sample_rate;
            int sample_size;
            int channel_count;
        };
    };

    int max_plane;

    struct {
        int stride;
        size_t size;
        uint8_t *data;
    }plane[4];

    int64_t timestamp;

    size_t size;

    uint8_t *data;

}sr_buffer_frame_t;

int sr_buffer_frame_set_image_format(sr_buffer_frame_t *frame, const uint8_t *data, int width,
                                     int height, uint32_t fourcc);
int sr_buffer_frame_convert_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int _rotation);
int sr_buffer_frame_convert_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst);


#endif //PROJECT_SR_BUFFER_FRAME_H
