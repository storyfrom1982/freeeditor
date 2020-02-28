//
// Created by yongge on 19-8-6.
//

#include "AVFramePool.h"

#include <sr_library.h>
#include <sr_malloc.h>

#include <libavutil/frame.h>



struct AVFramePool {
    int capacity;
    AVFrame config;
    sr_queue_t *sourceQueue;
    sr_queue_t *recycleQueue;
};


typedef struct NodeFrame {
    sr_node_t node;
    AVFramePool *pool;
    size_t ref;
    AVFrameEx frameEx;
}NodeFrame;


#define __frame2node(frameEx)    (NodeFrame*)((char*)(frameEx) - (sizeof(NodeFrame) - sizeof(AVFrameEx)))


static NodeFrame* alloc_node_frame(AVFrame *frame)
{
    NodeFrame *node = (NodeFrame *)malloc(sizeof(NodeFrame));
    if (!node){
        LOGE("malloc failed\n");
        return NULL;
    }

    node->frameEx.frame = av_frame_alloc();
    if (!node->frameEx.frame){
        LOGE("av_frame_alloc failed\n");
        free(node);
        return NULL;
    }

    if (frame){
        *node->frameEx.frame = *frame;
        if (av_frame_get_buffer(node->frameEx.frame, 0) != 0){
            LOGE("av_frame_get_buffer failed\n");
            av_frame_free(&node->frameEx.frame);
            free(node);
            return NULL;
        }
        LOGD("linesize: %d %d %d\n", node->frameEx.frame->linesize[0], node->frameEx.frame->linesize[1], node->frameEx.frame->linesize[2]);
    }

    return node;
}


static void free_node_frame(sr_node_t *node)
{
    LOGD("free_node_frame: %p\n", node);
    NodeFrame *nodeFrame = (NodeFrame *)node;
    if (nodeFrame){
        if (nodeFrame->frameEx.frame){
            av_frame_free(&(nodeFrame->frameEx.frame));
        }
        free(nodeFrame);
    }
}


static int av_frame_pool_initialize(AVFramePool *pool, AVFrame *frame)
{
    NodeFrame *node = NULL;
    for (int i = 0; i < pool->capacity; ++i){
        node = alloc_node_frame(frame);
        if (!node){
            LOGE("alloc_node_frame failed\n");
            return -1;
        }
        node->pool = pool;
        sr_queue_block_push_back(pool->sourceQueue, (sr_node_t*)node);
    }
    return 0;
}


AVFramePool* av_frame_pool_create(AVFrame *config, int capacity)
{
    LOGD("av_frame_pool_create enter\n");

    AVFramePool *pool = calloc(1, sizeof(AVFramePool));
    if (!pool){
        LOGE("calloc failed\n");
        return NULL;
    }

    pool->sourceQueue = sr_queue_create(free_node_frame);
    if (!pool->sourceQueue){
        LOGE("sr_queue_create failed\n");
        av_frame_pool_release(&pool);
        return NULL;
    }

    pool->recycleQueue = sr_queue_create(free_node_frame);
    if (!pool->recycleQueue){
        LOGE("sr_queue_create failed\n");
        av_frame_pool_release(&pool);
        return NULL;
    }

    if (config){
        pool->config = *config;
    }

    pool->capacity = capacity;

    int result = av_frame_pool_initialize(pool, config);
    if (result != 0){
        LOGE("av_frame_pool_initialize failed\n");
        av_frame_pool_release(&pool);
        return NULL;
    }

    return pool;
}

void av_frame_pool_release(AVFramePool **pp_pool)
{
    LOGD("av_frame_pool_release enter\n");
    if (pp_pool && *pp_pool){
        AVFramePool *pool = *pp_pool;
        *pp_pool = NULL;
        if (pool->recycleQueue){
            LOGD("av_frame_pool_release recycleQueue\n");
            sr_queue_release(&pool->recycleQueue);
        }
        if (pool->sourceQueue){
            LOGD("av_frame_pool_release sourceQueue\n");
            sr_queue_release(&pool->sourceQueue);
        }
        free(pool);
    }
    LOGD("av_frame_pool_release exit\n");
}

AVFrameEx* av_frame_pool_alloc(AVFramePool *pool)
{
    NodeFrame *node = NULL;
    if (!pool){
        LOGE("null pool\n");
        return NULL;
    }

    if (sr_queue_block_pop_front(pool->sourceQueue, (sr_node_t**)&node) != 0){
        LOGE("sr_queue_block_pop_back failed\n");
        return NULL;
    }

    if (sr_queue_block_push_back(pool->recycleQueue, (sr_node_t*)node) != 0){
        LOGE("sr_queue_block_push_back failed\n");
        return NULL;
    }

    node->ref = 1;

    return &node->frameEx;
}

void av_frame_pool_free(AVFrameEx *frameEx)
{
    if (!frameEx){
        LOGE("null frameEx\n");
        return;
    }

    NodeFrame *node = __frame2node(frameEx);
    if (&node->frameEx != frameEx){
        LOGE("invalid frameEx\n");
        return;
    }

    if (__sr_atom_sub(node->ref, 1) == 0){
        sr_queue_lock(node->pool->recycleQueue);
        sr_queue_remove_node(node->pool->recycleQueue, (sr_node_t*)node);
        sr_queue_unlock(node->pool->recycleQueue);
        sr_queue_block_push_back(node->pool->sourceQueue, (sr_node_t*)node);
    }
}

int av_frame_pool_add_reference(AVFrameEx *frameEx)
{
    if (!frameEx){
        LOGE("null frameEX\n");
        return 0;
    }

    NodeFrame *node = __frame2node(frameEx);
    if (&node->frameEx != frameEx) {
        LOGE("invalid frameEx\n");
        return 0;
    }

    return __sr_atom_add(node->ref, 1);
}