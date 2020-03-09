//
// Created by yongge on 19-7-2.
//

#ifndef PROJECT_SR_BUFFER_FRAME_H
#define PROJECT_SR_BUFFER_FRAME_H


#include <libyuv.h>
#include <sr_library.h>


int sr_buffer_frame_set_image_format(sr_buffer_frame_t *frame, const uint8_t *data, int width,
                                     int height, uint32_t fourcc);
int sr_buffer_frame_convert_to_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst, int _rotation);
int sr_buffer_frame_convert_from_yuv420p(sr_buffer_frame_t *src, sr_buffer_frame_t *dst);


#endif //PROJECT_SR_BUFFER_FRAME_H
