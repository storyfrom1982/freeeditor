//
// Created by yongge on 20-2-10.
//

#ifndef ANDROID_SR_BUFFER_POOL_H
#define ANDROID_SR_BUFFER_POOL_H

#include <stddef.h>

typedef struct sr_buffer_data_t {
    unsigned char *head;
    unsigned char *data;
}sr_buffer_data_t;


typedef struct sr_buffer_pool sr_buffer_pool_t;

sr_buffer_pool_t* sr_buffer_pool_create(size_t buffer_count, size_t buffer_size);
void sr_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool);

sr_buffer_data_t* sr_buffer_pool_get(sr_buffer_pool_t *pool);
void sr_buffer_pool_put(sr_buffer_data_t *buffer);


#endif //ANDROID_SR_BUFFER_POOL_H
