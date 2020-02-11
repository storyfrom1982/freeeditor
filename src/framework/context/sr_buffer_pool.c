//
// Created by yongge on 20-2-10.
//

#include "sr_buffer_pool.h"


#include <sr_malloc.h>
#include <sr_library.h>


struct sr_buffer_pool {
    unsigned int count, get_index, put_index;
    sr_buffer_t *buf_array;
};

sr_buffer_pool_t* sr_buffer_pool_create(unsigned int buffer_count){
    sr_buffer_pool_t *pool = (sr_buffer_pool_t*) malloc(sizeof(sr_buffer_pool_t));
    pool->count = buffer_count;
    pool->put_index = pool->get_index = 0;
    pool->buf_array = (sr_buffer_t*) calloc(sizeof(sr_buffer_t), buffer_count);
    return pool;
}

void sr_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool){
    if (pp_buffer_pool && *pp_buffer_pool){
        sr_buffer_pool_t *pool = *pp_buffer_pool;
        free(pool->buf_array);
        free(pool);
    }
}

unsigned int sr_buffer_pool_fill(sr_buffer_pool_t *pool, void *buffer){
    if ((pool->count - pool->put_index + pool->get_index) > 0){
        pool->buf_array[pool->put_index & (pool->count - 1)].p = buffer;
        pool->buf_array[pool->put_index & (pool->count - 1)].pool = pool;
        pool->put_index ++;
    }
    LOGD("fill pool put_index:%u\n", pool->put_index);
    return (pool->count - pool->put_index + pool->get_index);
}

unsigned int sr_buffer_pool_getable(sr_buffer_pool_t *pool){
    return (pool->put_index - pool->get_index);
}

sr_buffer_t* sr_buffer_pool_get(sr_buffer_pool_t *pool){
    if ((pool->put_index - pool->get_index) > 0){
        pool->get_index ++;
        LOGD("get pool get_index:%u\n", pool->get_index);
        return &pool->buf_array[pool->get_index & (pool->count - 1)];
    }
    return NULL;
}

void sr_buffer_pool_free(sr_buffer_t *buffer){
    sr_buffer_pool_t *pool = buffer->pool;
    if ((pool->count - pool->put_index + pool->get_index) > 0){
        pool->put_index ++;
        LOGD("put pool put_index:%u get_index:%u\n", pool->put_index, pool->get_index);
    }
}