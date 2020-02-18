//
// Created by yongge on 20-2-10.
//

#include "sr_buffer_pool.h"


#include <sr_malloc.h>
#include <sr_library.h>


typedef struct sr_buffer_node_t{
    sr_node_t node;
    sr_buffer_data_t buffer;
    sr_buffer_pool_t *pool;
}sr_buffer_node_t;

struct sr_buffer_pool {
    size_t size;
    size_t count;
    sr_queue_t *queue;
    sr_buffer_node_t *buffers;
};

sr_buffer_pool_t* sr_buffer_pool_create(size_t buffer_count, size_t buffer_size){
    sr_buffer_pool_t *pool = (sr_buffer_pool_t*) malloc(sizeof(sr_buffer_pool_t));
    pool->count = buffer_count;
    pool->size = buffer_size;
    pool->queue = sr_queue_create(pool->count, NULL);
    pool->buffers = (sr_buffer_node_t*)calloc(pool->count, sizeof(sr_buffer_node_t));
    for (int i = 0; i < pool->count; ++i){
        sr_buffer_node_t *node = pool->buffers + i;
        node->buffer.head = (uint8_t*)malloc(pool->size + 32);
        node->buffer.data = node->buffer.head + 32;
        node->pool = pool;
        __sr_queue_push_back(pool->queue, node);
    }
    return pool;
}

void sr_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool){
    if (pp_buffer_pool && *pp_buffer_pool){
        sr_buffer_pool_t *pool = *pp_buffer_pool;
        for (int i = 0; i < pool->count; ++i){
            free(pool->buffers[i].buffer.head);
        }
        sr_queue_release(&pool->queue);
        free(pool->buffers);
        free(pool);
    }
}

sr_buffer_data_t* sr_buffer_pool_get(sr_buffer_pool_t *pool){
    if (sr_queue_popable(pool->queue) > 0){
        sr_buffer_node_t *node;
        __sr_queue_block_pop_front(pool->queue, node);
        return &node->buffer;
    }
    return NULL;
}

void sr_buffer_pool_put(sr_buffer_data_t *buffer){
    if (buffer){
        sr_buffer_node_t *node = (sr_buffer_node_t*)((char *)buffer - sizeof(sr_node_t));
        __sr_queue_block_push_back(node->pool->queue, node);
    }else {
        LOGD("null parameter\n");
    }
}