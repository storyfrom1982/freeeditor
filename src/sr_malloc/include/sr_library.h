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

#define	__is_true(x)		__sync_bool_compare_and_swap(&(x), true, true)
#define	__is_false(x)		__sync_bool_compare_and_swap(&(x), false, false)
#define	__set_true(x)		__sync_bool_compare_and_swap(&(x), false, true)
#define	__set_false(x)		__sync_bool_compare_and_swap(&(x), true, false)

#define __sr_atom_sub(x, y)		__sync_sub_and_fetch(&(x), (y))
#define __sr_atom_add(x, y)		__sync_add_and_fetch(&(x), (y))
#define __sr_atom_lock(x)		while(!__set_true(x)) nanosleep((const struct timespec[]){{0, 1000L}}, NULL)
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
extern void sr_mutex_remove(sr_mutex_t **pp_mutex);

extern void sr_mutex_lock(sr_mutex_t *mutex);
extern void sr_mutex_unlock(sr_mutex_t *mutex);

extern void sr_mutex_wait(sr_mutex_t *mutex);
extern void sr_mutex_signal(sr_mutex_t *mutex);
extern void sr_mutex_broadcast(sr_mutex_t *mutex);


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////

enum {
    QUEUE_RESULT_USER_INTERRUPT = -3,
    QUEUE_RESULT_TRY_AGAIN = -2,
    QUEUE_RESULT_ERROR = -1,
	QUEUE_RESULT_OK = 0
};

typedef struct sr_node_t{
    struct sr_node_t *prev;
    struct sr_node_t *next;
}sr_node_t;

typedef struct sr_queue_t sr_queue_t;

extern sr_queue_t* sr_queue_create(int max_node_number, void (*free_node_cb)(sr_node_t*));
extern void sr_queue_release(sr_queue_t **pp_queue);

extern int sr_queue_push_front(sr_queue_t *queue, sr_node_t *node);
extern int sr_queue_push_back(sr_queue_t *queue, sr_node_t *node);

extern int sr_queue_pop_front(sr_queue_t *queue, sr_node_t **pp_node);
extern int sr_queue_pop_back(sr_queue_t *queue, sr_node_t **pp_node);

extern int sr_queue_remove_node(sr_queue_t *queue, sr_node_t *node);

extern void sr_queue_clean(sr_queue_t *queue);
extern void sr_queue_stop(sr_queue_t *queue);
extern bool sr_queue_is_stopped(sr_queue_t *queue);
extern void sr_queue_finish(sr_queue_t *queue);

extern void sr_queue_lock(sr_queue_t *queue);
extern void sr_queue_unlock(sr_queue_t *queue);

extern sr_node_t* sr_queue_get_first(sr_queue_t *queue);
extern sr_node_t* sr_queue_get_last(sr_queue_t *queue);

extern int sr_queue_pushable(sr_queue_t *queue);
extern int sr_queue_popable(sr_queue_t *queue);

extern int sr_queue_block_push_front(sr_queue_t *queue, sr_node_t *node);
extern int sr_queue_block_push_back(sr_queue_t *queue, sr_node_t *node);

extern int sr_queue_block_pop_front(sr_queue_t *queue, sr_node_t **pp_node);
extern int sr_queue_block_pop_back(sr_queue_t *queue, sr_node_t **pp_node);

extern void sr_queue_block_clean(sr_queue_t *queue);


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
extern void sr_pipe_remove(sr_pipe_t **pp_pipe);

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

enum {
	MessageType_Unknown = -2,
	MessageType_Pointer = -1,
	MessageType_Command = 0,
	MessageType_String = 1,
};

typedef struct sr_message{
	int32_t key;
	int32_t size;
	union {
        void *ptr; //size > 0 is string; size == 0 is object pointer
		int64_t i64;
        int32_t i32[2];
	};
}sr_message_t;

#define __sr_null_msg       ((sr_message_t){0})
#define __sr_error_msg       ((sr_message_t){-1, -1, 0})

typedef struct sr_msg_processor_t{
    const char *name;
	void *handler;
	void (*process)(struct sr_msg_processor_t *processor, sr_message_t msg);
}sr_message_processor_t;

typedef struct sr_message_queue sr_message_queue_t;

extern sr_message_queue_t* sr_message_queue_create();
extern void sr_message_queue_release(sr_message_queue_t **pp_queue);
extern int sr_message_queue_start_processor(sr_message_queue_t *queue, sr_message_processor_t *processor);

extern int sr_message_queue_get(sr_message_queue_t *queue, sr_message_t *msg);
extern int sr_message_queue_put(sr_message_queue_t *queue, sr_message_t msg);

extern unsigned int sr_message_queue_getable(sr_message_queue_t *queue);
extern unsigned int sr_message_queue_putable(sr_message_queue_t *queue);
extern void sr_message_queue_clear(sr_message_queue_t *queue);


///////////////////////////////////////////////////////////////
////signal
///////////////////////////////////////////////////////////////

extern void sr_setup_crash_backtrace();

#endif /* SR_LIBRARY_H_ */
