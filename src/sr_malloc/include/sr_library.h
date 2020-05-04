/*
 * sr_lib.h
 *
 *  Created on: 2018年12月7日
 *      Author: yongge
 */

#ifndef SR_LIBRARY_H_
#define SR_LIBRARY_H_

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

///////////////////////////////////////////////////////////////
////时间
///////////////////////////////////////////////////////////////

static inline int64_t sr_time_begin()
{
	struct timeval tv = {0};
	if (gettimeofday(&tv, NULL) != 0){
		assert(0);
	}
	return (1000000LL * tv.tv_sec + tv.tv_usec);
}

static inline int64_t sr_time_passed(int64_t begin_microsecond)
{
	struct timeval tv = {0};
	if (gettimeofday(&tv, NULL) != 0){
		assert(0);
	}
	return ((1000000LL * tv.tv_sec + tv.tv_usec) - begin_microsecond);
}

///////////////////////////////////////////////////////////////
////原子
///////////////////////////////////////////////////////////////

#define	__is_false(x)		__sync_bool_compare_and_swap(&(x), false, false)
#define	__is_true(x)		(!__is_false(x))
#define	__set_true(x)		__sync_bool_compare_and_swap(&(x), false, true)
#define	__set_false(x)		__sync_bool_compare_and_swap(&(x), true, false)

#define __sr_atom_sub(x, y)		__sync_sub_and_fetch(&(x), (y))
#define __sr_atom_add(x, y)		__sync_add_and_fetch(&(x), (y))
#define __sr_atom_lock(x)		while(!__set_true(x)) nanosleep((const struct timespec[]){{0, 100000L}}, NULL)
#define __sr_atom_trylock(x)	__set_true(x)
#define __sr_atom_unlock(x)		__set_false(x)


typedef union
{
	int32_t int32;
	float float32;
}if32_t;

typedef union
{
	int64_t int64;
	double float64;
}if64_t;

#define __sr_pop_int8(p, i) \
	(i) = (*(unsigned char *)(p)++)

#define __sr_pop_int16(p, i) \
	(i) \
	= (*(unsigned char *)(p)++ << 8) \
	| (*(unsigned char *)(p)++ << 0)

#define __sr_pop_int24(p, i) \
	(i) \
	= (*(unsigned char *)(p)++ << 16) \
	| (*(unsigned char *)(p)++ << 8) \
	| (*(unsigned char *)(p)++ << 0)

#define __sr_pop_int32(p, i) \
	(i) \
	= (*(unsigned char *)(p)++ << 24) \
	| (*(unsigned char *)(p)++ << 16) \
	| (*(unsigned char *)(p)++ << 8) \
	| (*(unsigned char *)(p)++ << 0)

#define __sr_pop_int64(p, i) \
	(i) \
	= ((int64_t)*(unsigned char *)(p)++ << 56) \
	| ((int64_t)*(unsigned char *)(p)++ << 48) \
	| ((int64_t)*(unsigned char *)(p)++ << 40) \
	| ((int64_t)*(unsigned char *)(p)++ << 32) \
	| ((int64_t)*(unsigned char *)(p)++ << 24) \
	| ((int64_t)*(unsigned char *)(p)++ << 16) \
	| ((int64_t)*(unsigned char *)(p)++ << 8) \
	| ((int64_t)*(unsigned char *)(p)++ << 0)


#define __sr_push_int8(p, i) \
	(*p++) = (unsigned char)(i)

#define __sr_push_int16(p, i) \
	*(p)++ = (unsigned char)((i) >> 8); \
	*(p)++ = (unsigned char)((i) >> 0)

#define __sr_push_int24(p, i) \
	*(p)++ = (unsigned char)((i) >> 16); \
	*(p)++ = (unsigned char)((i) >> 8); \
	*(p)++ = (unsigned char)((i) >> 0)

#define __sr_push_int32(p, i) \
	*(p)++ = (unsigned char)((i) >> 24); \
	*(p)++ = (unsigned char)((i) >> 16); \
	*(p)++ = (unsigned char)((i) >> 8); \
	*(p)++ = (unsigned char)((i) >> 0)

#define __sr_push_int64(p, i) \
	*(p)++ = (unsigned char)((int64_t)(i) >> 56); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 48); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 40); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 32); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 24); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 16); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 8); \
	*(p)++ = (unsigned char)((int64_t)(i) >> 0)

///////////////////////////////////////////////////////////////
////日志
///////////////////////////////////////////////////////////////

enum{
	SR_LOG_LEVEL_DEBUG = 0,
    SR_LOG_LEVEL_INFO,
	SR_LOG_LEVEL_WARN,
	SR_LOG_LEVEL_ERROR,
    SR_LOG_LEVEL_FATAL
};

extern int sr_log_file_open(const char *log_directory);
extern void sr_log_file_close();

extern void sr_log_set_callback(int level, void (*cb)(int level, const char *debug_log, const char *pure_log));
extern void sr_log_msg(const char *fmt, ...);
extern void sr_log_debug(int level, const char *file, const char *func, int line, const char *fmt, ...);


#ifdef LOGI
# undef LOGI
#endif
#define LOGI(__FORMAT__, ...) \
    sr_log_debug(SR_LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, __FORMAT__, ##__VA_ARGS__)


#ifdef SR_LOG_DEBUG
# ifdef LOGD
#  undef LOGD
# endif
# define LOGD(__FORMAT__, ...) \
    sr_log_debug(SR_LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, __FORMAT__, ##__VA_ARGS__)
#else
# define LOGD(__FORMAT__, ...)	do {} while(0)
#endif


#ifdef LOGW
# undef LOGW
#endif
#define LOGW(__FORMAT__, ...) \
    sr_log_debug(SR_LOG_LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, __FORMAT__, ##__VA_ARGS__)


#ifdef LOGE
 #undef LOGE
#endif
#define LOGE(__FORMAT__, ...) \
    sr_log_debug(SR_LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, __FORMAT__, ##__VA_ARGS__)


#ifdef LOGF
# undef LOGF
#endif
#define LOGF(__FORMAT__, ...) \
    sr_log_debug(SR_LOG_LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__, __FORMAT__, ##__VA_ARGS__)


///////////////////////////////////////////////////////////////
////线程
///////////////////////////////////////////////////////////////

#include <pthread.h>

typedef struct sr_mutex_t sr_mutex_t;

extern sr_mutex_t* sr_mutex_create();
extern void sr_mutex_release(sr_mutex_t **pp_mutex);

extern void sr_mutex_lock(sr_mutex_t *mutex);
extern void sr_mutex_unlock(sr_mutex_t *mutex);

extern void sr_mutex_wait(sr_mutex_t *mutex);
extern void sr_mutex_signal(sr_mutex_t *mutex);
extern void sr_mutex_broadcast(sr_mutex_t *mutex);


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////

enum {
    QUEUE_RESULT_ERROR_EMPTY = -2,
    QUEUE_RESULT_ERROR_STOPPED = -1,
};

typedef struct sr_node_t{
    struct sr_node_t *prev;
    struct sr_node_t *next;
}sr_node_t;

typedef struct sr_queue_t sr_queue_t;

extern sr_queue_t* sr_queue_create(void (*clear_cb)(sr_node_t*));
extern void sr_queue_release(sr_queue_t **pp_queue);

extern int sr_queue_push_front(sr_queue_t *queue, sr_node_t *node);
extern int sr_queue_push_back(sr_queue_t *queue, sr_node_t *node);

extern int sr_queue_pop_front(sr_queue_t *queue, sr_node_t **pp_node);
extern int sr_queue_pop_back(sr_queue_t *queue, sr_node_t **pp_node);

extern int sr_queue_remove_node(sr_queue_t *queue, sr_node_t *node);

extern void sr_queue_clear(sr_queue_t *queue);
extern void sr_queue_stop(sr_queue_t *queue);
extern bool sr_queue_is_stopped(sr_queue_t *queue);

extern void sr_queue_lock(sr_queue_t *queue);
extern void sr_queue_unlock(sr_queue_t *queue);

extern sr_node_t* sr_queue_get_first(sr_queue_t *queue);
extern sr_node_t* sr_queue_get_last(sr_queue_t *queue);

extern int sr_queue_length(sr_queue_t *queue);

extern int sr_queue_block_push_front(sr_queue_t *queue, sr_node_t *node);
extern int sr_queue_block_push_back(sr_queue_t *queue, sr_node_t *node);

extern int sr_queue_block_pop_front(sr_queue_t *queue, sr_node_t **pp_node);
extern int sr_queue_block_pop_back(sr_queue_t *queue, sr_node_t **pp_node);

extern void sr_queue_block_clear(sr_queue_t *queue);


#define __sr_queue_push_front(queue, node) \
		sr_queue_push_front((queue), (sr_node_t*)(node))

#define __sr_queue_push_back(queue, node) \
		sr_queue_push_back((queue), (sr_node_t*)(node))

#define __sr_queue_pop_front(queue, node) \
		sr_queue_pop_front((queue), (sr_node_t**)(&(node)))

#define __sr_queue_pop_back(queue, node) \
		sr_queue_pop_back((queue), (sr_node_t**)(&(node)))

#define __sr_queue_remove_node(queue, node) \
		sr_queue_remove_node((queue), (sr_node_t*)(node))

#define __sr_queue_block_push_front(queue, node) \
		sr_queue_block_push_front((queue), (sr_node_t*)(node))

#define __sr_queue_block_push_back(queue, node) \
		sr_queue_block_push_back((queue), (sr_node_t*)(node))

#define __sr_queue_block_pop_front(queue, node) \
		sr_queue_block_pop_front((queue), (sr_node_t**)(&(node)))

#define __sr_queue_block_pop_back(queue, node) \
		sr_queue_block_pop_back((queue), (sr_node_t**)(&(node)))


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////


typedef struct sr_pipe_t sr_pipe_t;

extern sr_pipe_t* sr_pipe_create(unsigned int size);
extern void sr_pipe_release(sr_pipe_t **pp_pipe);

extern void sr_pipe_complete(sr_pipe_t *pipe);
extern void sr_pipe_stop(sr_pipe_t *pipe);
extern bool sr_pipe_is_stopped(sr_pipe_t *pipe);

extern void sr_pipe_clean(sr_pipe_t *pipe);
extern void sr_pipe_reset(sr_pipe_t *pipe);

extern unsigned int sr_pipe_writable(sr_pipe_t *pipe);
extern unsigned int sr_pipe_readable(sr_pipe_t *pipe);

extern unsigned int sr_pipe_read(sr_pipe_t *pipe, char *buf, unsigned int size);
extern unsigned int sr_pipe_write(sr_pipe_t *pipe, char *data, unsigned int size);

extern unsigned int sr_pipe_block_read(sr_pipe_t *pipe, char *buf, unsigned int size);
extern unsigned int sr_pipe_block_write(sr_pipe_t *pipe, char *data, unsigned int size);


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////

#define SR_MSG_MAX_BUFFER_SIZE     4096

typedef struct sr_msg_type_t {
    int key;
    float f32;
    double f64;
    int32_t i32;
	int64_t i64;
    void *obj;
    size_t size;
    unsigned char *data;
}sr_msg_type_t;


typedef struct sr_msg_frame_t {

    int type;
    int flag;
    int stream_id;

    int x;
    int y;

    int width;
    int height;
    int video_format;

    int sample_rate;
    int audio_format;
    int audio_channel;

    struct {
        int x;
        int y;
        int width;
        int height;
        int frame_rate;
        int image_format;
    }video;

    struct {
        int sample_rate;
        int sample_size;
        int sample_format;
        int channel_count;
    }audio;

    int size;
    unsigned char *data;

    struct {
        int size;
        int stride;
        unsigned char *data;
    }channel[4];

	int channel_count;

    int64_t timestamp;

}sr_msg_frame_t;


typedef struct sr_msg_buffer_t {
    void *context;
    size_t align;
    size_t head_size;
    size_t data_size;
    unsigned char *head;
    unsigned char *data;
}sr_msg_buffer_t;


typedef struct sr_msg_t {
    sr_msg_type_t type;
    sr_msg_frame_t frame;
    sr_msg_buffer_t buffer;
    int reference_count;
    void (*recycle)(struct sr_msg_t *msg);
    void (*realloc)(struct sr_msg_t *msg, size_t size);
}sr_msg_t;

static inline void sr_msg_reference_add(sr_msg_t *msg){
    assert(msg != NULL);
    __sr_atom_add(msg->reference_count, 1);
}

static inline void sr_msg_reference_sub(sr_msg_t *msg){
    assert(msg != NULL);
    if (__sr_atom_sub(msg->reference_count, 1) == 0){
        if (msg->recycle){
            msg->recycle(msg);
        }
    }
}

typedef struct sr_msg_buffer_pool sr_msg_buffer_pool_t;

sr_msg_buffer_pool_t* sr_msg_buffer_pool_create(
        const char *name,
        size_t msg_count,
        size_t max_count,
        size_t msg_buffer_size,
        size_t msg_buffer_head_size,
        size_t msg_buffer_data_align);
void sr_msg_buffer_pool_release(sr_msg_buffer_pool_t **pp_msg_pool);
sr_msg_t* sr_msg_buffer_pool_alloc(sr_msg_buffer_pool_t *pool);


///////////////////////////////////////////////////////////////
////signal
///////////////////////////////////////////////////////////////

extern void sr_setup_crash_backtrace();

#endif /* SR_LIBRARY_H_ */
