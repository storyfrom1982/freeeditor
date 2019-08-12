//
// Created by yongge on 19-8-5.
//

#ifndef ANDROID_AVFRAMEPOOL_H
#define ANDROID_AVFRAMEPOOL_H

#include <libavutil/frame.h>

struct AVFrame;

typedef struct AVFrameEx{
    struct AVFrame *frame;
}AVFrameEx;

typedef struct AVFramePool AVFramePool;

AVFramePool* av_frame_pool_create(struct AVFrame *config, int capacity);
void av_frame_pool_release(AVFramePool **pp_pool);

AVFrameEx* av_frame_pool_alloc(AVFramePool *pool);
void av_frame_pool_free(AVFrameEx *frameEx);
int av_frame_pool_add_reference(AVFrameEx *frameEx);


#endif //ANDROID_AVFRAMEPOOL_H
