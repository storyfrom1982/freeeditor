//
// Created by yongge on 19-7-2.
//

#ifndef LIBYUV_RESAMPLER_H
#define LIBYUV_RESAMPLER_H


#include <libyuv.h>
#include <sr_library.h>

uint32_t libyuv_convert_fourcc(const char *p_fourcc);
int libyuv_set_format(sr_message_frame_t *frame, const uint8_t *data, int width, int height, uint32_t fourcc);
int libyuv_convert_to_yuv420p(sr_message_frame_t *src, sr_message_frame_t *yuv420p, int _rotation);
int libyuv_convert_from_yuv420p(sr_message_frame_t *yuv420p, sr_message_frame_t *dst);


#endif //LIBYUV_RESAMPLER_H
