//
// Created by yongge on 20-2-10.
//

#ifndef ANDROID_SR_BUFFER_POOL_H
#define ANDROID_SR_BUFFER_POOL_H

typedef struct sr_buffer_pool sr_buffer_pool_t;

typedef struct sr_buffer{
    int reference_count;
    sr_buffer_pool_t *pool;
    void *ptr;
}sr_buffer_t;

sr_buffer_pool_t* sr_buffer_pool_create(unsigned int buffer_count);
void sr_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool);

unsigned int sr_buffer_pool_fill(sr_buffer_pool_t *pool, void *p);
unsigned int sr_buffer_pool_getable(sr_buffer_pool_t *pool);

sr_buffer_t* sr_buffer_pool_get(sr_buffer_pool_t *pool);
void sr_buffer_pool_free(sr_buffer_t *buffer);


#endif //ANDROID_SR_BUFFER_POOL_H
