/*
 * sr_lib.c
 *
 *  Created on: 2018年12月7日
 *      Author: yongge
 */


#include "sr_library.h"
#include "sr_malloc.h"


///////////////////////////////////////////////////////////////
////日志
///////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


static const char *g_default_name = "default";

#define __log_text_size			1024

#define __path_clear(path) \
	( strrchr( path, '/' ) ? strrchr( path, '/' ) + 1 : path )

typedef void (*log_callback) (int level, const char *debug_log, const char *pure_log);


static void log_default_callback(int level, const char *debug_log, const char *pure_log)
{
    if (level == SR_LOG_LEVEL_DEBUG){
        fprintf(stdout, "DEBUG: %s", debug_log);
    }else if (level == SR_LOG_LEVEL_INFO){
        fprintf(stdout, "INFO: %s", debug_log);
    }else if (level == SR_LOG_LEVEL_WARN){
        fprintf(stdout, "WARN: %s", debug_log);
    }else if (level == SR_LOG_LEVEL_ERROR){
        fprintf(stderr, "ERROR: %s", debug_log);
    }else if (level == SR_LOG_LEVEL_FATAL){
        fprintf(stderr, "FATAL: %s", debug_log);
    }
}

static int g_log_level = SR_LOG_LEVEL_DEBUG;
static log_callback g_log_callback = log_default_callback;


void sr_log_set_callback(int level, void (*cb)(int level, const char *debug_log, const char *pure_log))
{
    g_log_level = level;
    g_log_callback = cb;
}


typedef struct sr_log_writer_t{
    bool running;
    pthread_t tid;
    char *dir_name;
    FILE *fp;
    sr_pipe_t *pipe;
}sr_log_writer_t;

static pthread_mutex_t g_log_writer_mutex = PTHREAD_MUTEX_INITIALIZER;

static sr_log_writer_t g_log_writer = {0};

#define __sr_log_pipe_size    4096

static void* log_file_writer_loop(void *p)
{
    size_t line_size = 0;
    size_t remain_size = 0;
    char text[__sr_log_pipe_size] = {0};
    sr_log_writer_t *writer = (sr_log_writer_t *)p;

    snprintf(text, __sr_log_pipe_size, "%s/media.0.log", writer->dir_name);
    writer->fp = fopen(text, "a+t");
    if (writer->fp == NULL){
        sr_log_msg("fopen failed %s\n", text);
        return NULL;
    }

    while (__is_true(writer->running)){
        line_size = sr_pipe_block_read(writer->pipe, text, 1);
        if (line_size == 1){
            remain_size = sr_pipe_readable(writer->pipe);
            if (remain_size > __sr_log_pipe_size - 1){
                remain_size = __sr_log_pipe_size - 1;
            }
            line_size += sr_pipe_block_read(writer->pipe, text + 1, remain_size);
            if (fwrite(text, 1, line_size, writer->fp) != line_size){
                sr_log_msg("fwrite failed\n");
                break;
            }
        }
        if (line_size == 0){
            __set_false(writer->running);
            sr_log_msg("log_file_writer_loop break\n");
            break;
        }
        fflush(writer->fp);
        if (ftell(writer->fp) > 1024 * 1024 * 4){
            fclose(writer->fp);
            memset(text, 0, __sr_log_pipe_size);
            snprintf(text, __sr_log_pipe_size, "%s/media.0.log", writer->dir_name);
            char file_name[1024] = {0};
            snprintf(file_name, 1024, "%s/media.1.log", writer->dir_name);
            rename(text, file_name);
            g_log_writer.fp = fopen(text, "w+t");
            if (writer->fp == NULL){
                sr_log_msg("fopen failed %s\n", text);
                return NULL;
            }
        }
    }

    if (writer->fp != NULL){
        fclose(writer->fp);
        writer->fp = NULL;
    }

    return NULL;
}

int sr_log_file_open(const char *log_directory)
{
    pthread_mutex_lock(&g_log_writer_mutex);
    if (__is_false(g_log_writer.running)){
        g_log_writer.dir_name = strdup(log_directory);
        g_log_writer.pipe = sr_pipe_create(__sr_log_pipe_size * 2);
        if (pthread_create(&(g_log_writer.tid), NULL, log_file_writer_loop, &g_log_writer) != 0){
            LOGE("pthread_create failed\n");
            free(g_log_writer.dir_name);
            sr_pipe_remove(&(g_log_writer.pipe));
            pthread_mutex_unlock(&g_log_writer_mutex);
            return -1;
        }
        __set_true(g_log_writer.running);
    }
    pthread_mutex_unlock(&g_log_writer_mutex);
    return 0;
}


void sr_log_file_close()
{
    pthread_mutex_lock(&g_log_writer_mutex);
    if (__set_false(g_log_writer.running)){
        sr_pipe_complete(g_log_writer.pipe);
        if (g_log_writer.tid != 0){
            pthread_join(g_log_writer.tid, NULL);
            g_log_writer.tid = 0;
        }
        if (g_log_writer.dir_name){
            free(g_log_writer.dir_name);
            g_log_writer.dir_name = NULL;
        }
        sr_pipe_remove(&(g_log_writer.pipe));
    }
    pthread_mutex_unlock(&g_log_writer_mutex);
}


static void sr_log_file_write(char *log, size_t size)
{
    if (__is_true(g_log_writer.running)){
        sr_pipe_block_write(g_log_writer.pipe, log, size);
    }
}


void sr_log_msg(const char *fmt, ...)
{
    char text[__log_text_size] = {0};
    va_list args;
    va_start (args, fmt);
    size_t n = (size_t) vsnprintf(text, __log_text_size, fmt, args);
    va_end (args);
    g_log_callback(SR_LOG_LEVEL_INFO, "", text);
    sr_log_file_write(text, n);
}


void sr_log_debug(int level, const char *path, const char *func, int line, const char *fmt, ...)
{
    if (level >= g_log_level){
        size_t n = 0;
        char text[__log_text_size] = {0};
        time_t t;
        time(&t);
        struct timeval now;
        gettimeofday(&now, NULL);
        n = strftime(text, __log_text_size, "%Y-%m-%d %H:%M:%S", localtime(&t));
        n += snprintf(text + n, __log_text_size - n, ".%03d [%s] %s(%d) ", (int) (now.tv_usec / 1000), __path_clear(path), func, line);
        char *pure_log = text + n;
        va_list args;
        va_start (args, fmt);
        n += vsnprintf(text + n, __log_text_size - n, fmt, args);
        va_end (args);
        if (g_log_callback){
            g_log_callback(level, text, pure_log);
        }
        if (__is_true(g_log_writer.running)){
            sr_log_file_write(text, n);
        }
        if (level == SR_LOG_LEVEL_FATAL){
            assert(0);
        }
    }
}


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////


struct sr_mutex_t{
	pthread_cond_t cond[1];
	pthread_mutex_t mutex[1];
};


sr_mutex_t* sr_mutex_create()
{
	sr_mutex_t *mutex = NULL;

	if ((mutex = malloc(sizeof(sr_mutex_t))) == NULL){
		LOGF("malloc failed\n");
	}

	if (pthread_cond_init(mutex->cond, NULL) != 0){
		LOGF("pthread_cond_init failed\n");
	}

	if (pthread_mutex_init(mutex->mutex, NULL) != 0){
		LOGF("pthread_mutex_init failed\n");
	}

	return mutex;
}

void sr_mutex_release(sr_mutex_t **pp_mutex)
{
	if (pp_mutex && *pp_mutex){
		sr_mutex_t *mutex = *pp_mutex;
		*pp_mutex = NULL;
		pthread_cond_destroy(mutex->cond);
		pthread_mutex_destroy(mutex->mutex);
		free(mutex);
	}
}

inline void sr_mutex_lock(sr_mutex_t *mutex)
{
    assert(mutex);
    if (pthread_mutex_lock(mutex->mutex) != 0){
        LOGF("pthread_mutex_lock failed\n");
    }
}

inline void sr_mutex_unlock(sr_mutex_t *mutex)
{
    assert(mutex);
    if (pthread_mutex_unlock(mutex->mutex) != 0){
        LOGF("pthread_mutex_unlock failed\n");
    }
}

inline void sr_mutex_wait(sr_mutex_t *mutex)
{
    assert(mutex);
    if (pthread_cond_wait(mutex->cond, mutex->mutex) != 0){
        LOGF("pthread_cond_wait failed\n");
    }
}

inline void sr_mutex_signal(sr_mutex_t *mutex)
{
    assert(mutex);
    if (pthread_cond_signal(mutex->cond) != 0){
        LOGE("pthread_cond_signal failed\n");
    }
}

inline void sr_mutex_broadcast(sr_mutex_t *mutex)
{
    assert(mutex);
    if (pthread_cond_broadcast(mutex->cond) != 0){
        LOGE("pthread_cond_broadcast failed\n");
    }
}


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////


struct sr_queue_t {
    int length;
    void (*clear_cb)(sr_node_t*);
    sr_node_t head;
    sr_node_t end;
    int waiting;
    bool stopped;
    sr_mutex_t *mutex;
};


sr_queue_t* sr_queue_create(void (*clear_cb)(sr_node_t*))
{
    sr_queue_t *queue = (sr_queue_t*)calloc(1, sizeof(sr_queue_t));
    if (queue == NULL){
        LOGE("calloc failed\n");
        return NULL;
    }

    if ((queue->mutex = sr_mutex_create()) == NULL){
        LOGE("sr_mutex_create failed\n");
        free(queue);
        return NULL;
    }

    queue->waiting = 0;

    queue->stopped = false;
    queue->length = 0;
    queue->head.prev = NULL;
    queue->head.next = &(queue->end);
    queue->end.next = NULL;
    queue->end.prev = &(queue->head);
    queue->clear_cb = clear_cb;

    return queue;
}

void sr_queue_release(sr_queue_t **pp_queue)
{
    if (pp_queue && *pp_queue){
        sr_queue_t *queue = *pp_queue;
        *pp_queue = NULL;
        sr_queue_block_clear(queue);
        sr_queue_stop(queue);
        sr_mutex_release(&queue->mutex);
        free(queue);
    }
}

int sr_queue_push_front(sr_queue_t *queue, sr_node_t *node)
{
    assert(queue != NULL && node != NULL);
    node->next = queue->head.next;
    node->next->prev = node;
    node->prev = &(queue->head);
    queue->head.next = node;
    __sr_atom_add(queue->length, 1);
    return queue->length;
}

int sr_queue_push_back(sr_queue_t *queue, sr_node_t *node)
{
    assert(queue != NULL && node != NULL);
//    assert(node != queue->end.prev);
    node->prev = queue->end.prev;
    node->next = &(queue->end);
    node->prev->next = node;
    queue->end.prev = node;
    __sr_atom_add(queue->length, 1);
    return queue->length;
}

int sr_queue_pop_front(sr_queue_t *queue, sr_node_t **pp_node)
{
    assert(queue != NULL && pp_node != NULL);
    if (queue->length == 0){
        return QUEUE_RESULT_ERROR_EMPTY;
    }
    (*pp_node) = queue->head.next;
//    assert(queue->head.next != &queue->end);
    queue->head.next = (*pp_node)->next;
    (*pp_node)->next->prev = &(queue->head);
    __sr_atom_sub(queue->length, 1);
    return queue->length;
}

int sr_queue_pop_back(sr_queue_t *queue, sr_node_t **pp_node)
{
    assert(queue != NULL && pp_node != NULL);
    if (queue->length == 0){
        return QUEUE_RESULT_ERROR_EMPTY;
    }
    (*pp_node) = queue->end.prev;
    queue->end.prev = (*pp_node)->prev;
    (*pp_node)->prev->next = &(queue->end);
    __sr_atom_sub(queue->length, 1);
    return queue->length;
}

int sr_queue_remove_node(sr_queue_t *queue, sr_node_t *node)
{
    assert(queue != NULL);
    if (queue->length > 0){
        assert(node != NULL && node->next != NULL && node->prev != NULL);
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->prev = node->next = NULL;
        __sr_atom_sub(queue->length, 1);
    }
    return queue->length;
}

void sr_queue_lock(sr_queue_t *queue)
{
    assert(queue != NULL);
    sr_mutex_lock(queue->mutex);
}

void sr_queue_unlock(sr_queue_t *queue)
{
    assert(queue != NULL);
    sr_mutex_unlock(queue->mutex);
}

sr_node_t* sr_queue_get_first(sr_queue_t *queue)
{
    assert(queue != NULL);
	return queue->head.next;
}

sr_node_t* sr_queue_get_last(sr_queue_t *queue)
{
    assert(queue != NULL);
	return queue->end.prev;
}

int sr_queue_length(sr_queue_t *queue)
{
    assert(queue != NULL);
    return queue->length;
}

void sr_queue_clear(sr_queue_t *queue)
{
    assert(queue != NULL);
    if ((queue)->clear_cb){
        while ((queue)->head.next != &((queue)->end)){
            (queue)->head.prev = (queue)->head.next;
            (queue)->head.next = (queue)->head.next->next;
            (queue)->length--;
            (queue)->clear_cb((queue)->head.prev);
        }
        assert((queue)->length == 0);
    }
	(queue)->head.prev = NULL;
	(queue)->head.next = &((queue)->end);
	(queue)->end.next = NULL;
	(queue)->end.prev = &((queue)->head);
}


bool sr_queue_is_stopped(sr_queue_t *queue)
{
    assert(queue != NULL);
    return __is_true(queue->stopped);
}


void sr_queue_stop(sr_queue_t *queue)
{
    assert(queue != NULL);
    if (__set_true(queue->stopped)){
        while (queue->waiting > 0){
            sr_mutex_lock(queue->mutex);
            sr_mutex_broadcast(queue->mutex);
            sr_mutex_unlock(queue->mutex);
            nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
        }
    }
}


int sr_queue_block_push_front(sr_queue_t *queue, sr_node_t *node)
{
    assert(queue != NULL && node != NULL);
    sr_mutex_lock(queue->mutex);
    int result = sr_queue_push_front(queue, node);
    if (queue->waiting > 0){
        sr_mutex_broadcast(queue->mutex);
    }
    sr_mutex_unlock(queue->mutex);
    return result;
}


int sr_queue_block_push_back(sr_queue_t *queue, sr_node_t *node)
{
    assert(queue != NULL && node != NULL);
    sr_mutex_lock(queue->mutex);
    int result = sr_queue_push_back(queue, node);
    if (queue->waiting > 0){
        sr_mutex_broadcast(queue->mutex);
    }
    sr_mutex_unlock(queue->mutex);
    return result;
}


int sr_queue_block_pop_front(sr_queue_t *queue, sr_node_t **pp_node)
{
    int result = 0;
    assert(queue != NULL && pp_node != NULL);
    sr_mutex_lock(queue->mutex);
    while ((result = sr_queue_pop_front(queue, pp_node)) == QUEUE_RESULT_ERROR_EMPTY){
        if (__is_false(queue->stopped)){
            __sr_atom_add(queue->waiting, 1);
            sr_mutex_wait(queue->mutex);
            __sr_atom_sub(queue->waiting, 1);
        }else {
            sr_mutex_unlock(queue->mutex);
            return QUEUE_RESULT_ERROR_STOPPED;
        }
    }
    sr_mutex_unlock(queue->mutex);
    return result;
}


int sr_queue_block_pop_back(sr_queue_t *queue, sr_node_t **pp_node)
{
    int result = 0;
    assert(queue != NULL && pp_node != NULL);
    sr_mutex_lock(queue->mutex);
    while ((result = sr_queue_pop_back(queue, pp_node)) == QUEUE_RESULT_ERROR_EMPTY){
        if (__is_false(queue->stopped)){
            __sr_atom_add(queue->waiting, 1);
            sr_mutex_wait(queue->mutex);
            __sr_atom_sub(queue->waiting, 1);
        }else {
            sr_mutex_unlock(queue->mutex);
            return QUEUE_RESULT_ERROR_STOPPED;
        }
    }
    sr_mutex_unlock(queue->mutex);
    return result;
}


void sr_queue_block_clear(sr_queue_t *queue)
{
    assert(queue != NULL);
    sr_mutex_lock(queue->mutex);
    sr_queue_clear(queue);
    sr_mutex_unlock(queue->mutex);
}


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////

struct sr_pipe_t {
	//non-blocking
	bool stopped;
	uint8_t *buf;
	unsigned int len;
	unsigned int writer;
	unsigned int reader;
    //blocking
    int write_waiting;
    int read_waiting;
    sr_mutex_t *mutex;
};



sr_pipe_t* sr_pipe_create(unsigned int len)
{
	sr_pipe_t *pipe = NULL;

	if ((pipe = (sr_pipe_t *)calloc(1, sizeof(sr_pipe_t))) == NULL){
		LOGF("calloc failed\n");
	}

    if (len == 0){
        pipe->len = 1 << 15;
    }else if ( ( len & ( len - 1 ) ) == 0 ){
		pipe->len = len;
	}else{
		pipe->len = 2;
		while(len >>= 1){
			pipe->len <<= 1;
		}
	}

	if ((pipe->buf = ( uint8_t * )malloc( pipe->len )) == NULL){
		LOGF("malloc failed\n");
	}

    pipe->mutex = sr_mutex_create();
    pipe->stopped = false;
	pipe->writer = pipe->reader = 0;
	pipe->read_waiting = pipe->write_waiting = 0;

	return pipe;
}


void sr_pipe_remove(sr_pipe_t **pp_pipe)
{
	if (pp_pipe && *pp_pipe){
		sr_pipe_t *pipe = *pp_pipe;
		*pp_pipe = NULL;
		sr_pipe_stop(pipe);
        sr_mutex_release(&pipe->mutex);
		free(pipe->buf);
		free(pipe);
	}
}


void sr_pipe_stop(sr_pipe_t *pipe)
{
	if (pipe != NULL){
		if (__set_true(pipe->stopped)){
            sr_pipe_clean(pipe);
            sr_mutex_lock(pipe->mutex);
            sr_mutex_broadcast(pipe->mutex);
            sr_mutex_unlock(pipe->mutex);
            while(!__is_false(pipe->write_waiting) || !__is_false(pipe->read_waiting)){
                nanosleep((const struct timespec[]){{0, 10000L}}, NULL);
                sr_mutex_lock(pipe->mutex);
                sr_mutex_broadcast(pipe->mutex);
                sr_mutex_unlock(pipe->mutex);
            }
		}
	}
}


void sr_pipe_complete(sr_pipe_t *pipe)
{
	if (pipe != NULL){
		__set_true(pipe->stopped);
        sr_mutex_lock(pipe->mutex);
        sr_mutex_broadcast(pipe->mutex);
        sr_mutex_unlock(pipe->mutex);
	}
}


bool sr_pipe_is_stopped(sr_pipe_t *pipe)
{
    if (pipe != NULL){
        return pipe->stopped;
    }
    return true;
}


void sr_pipe_clean(sr_pipe_t *pipe)
{
	if (pipe){
		pipe->writer = pipe->reader = 0;
	}
}


void sr_pipe_reset(sr_pipe_t *pipe)
{
    if (pipe != NULL){
        sr_pipe_clean(pipe);
        __set_false(pipe->stopped);
    }
}


unsigned int sr_pipe_writable(sr_pipe_t *pipe)
{
	if (pipe){
		return pipe->len - pipe->writer + pipe->reader;
	}
	return 0;
}


unsigned int sr_pipe_readable(sr_pipe_t *pipe)
{
	if (pipe){
		return pipe->writer - pipe->reader;
	}
	return 0;
}


unsigned int sr_pipe_write(sr_pipe_t *pipe, char *data, unsigned int size)
{
	if (pipe == NULL || data == NULL || size == 0){
		LOGE("Invalid parameter: pipe=%p data=%p size=%u\n", pipe, data, size);
		return 0;
	}

	unsigned int writable = pipe->len - pipe->writer + pipe->reader;
	unsigned int remain = pipe->len - ( pipe->writer & ( pipe->len - 1 ) );

	if ( writable == 0 ){
		return 0;
	}

	size = writable < size ? writable : size;

	if ( remain >= size ){
		memcpy( pipe->buf + ( pipe->writer & ( pipe->len - 1 ) ), data, size);
	}else{
		memcpy( pipe->buf + ( pipe->writer & ( pipe->len - 1 ) ), data, remain);
		memcpy( pipe->buf, data + remain, size - remain);
	}

	__sr_atom_add( pipe->writer, size );

	return size;
}


unsigned int sr_pipe_read(sr_pipe_t *pipe, char *buf, unsigned int size)
{
	if (pipe == NULL || buf == NULL || size == 0){
        LOGE("Invalid parameter: pipe=%p buf=%p size=%u\n", pipe, buf, size);
		return 0;
	}

	unsigned int readable = pipe->writer - pipe->reader;
	unsigned int remain = pipe->len - ( pipe->reader & ( pipe->len - 1 ) );

	if ( readable == 0 ){
		return 0;
	}

	size = readable < size ? readable : size;

	if ( remain >= size ){
		memcpy( buf, pipe->buf + ( pipe->reader & ( pipe->len - 1 ) ), size);
	}else{
		memcpy( buf, pipe->buf + ( pipe->reader & ( pipe->len - 1 ) ), remain);
		memcpy( buf + remain, pipe->buf, size - remain);
	}

	__sr_atom_add( pipe->reader, size );

	return size;
}


unsigned int sr_pipe_block_write(sr_pipe_t *pipe, char *data, unsigned int size)
{
	if (pipe == NULL || data == NULL || size == 0){
        LOGE("Invalid parameter: pipe=%p data=%p size=%u\n", pipe, data, size);
		return 0;
	}

	if (__is_true(pipe->stopped)){
		return 0;
	}

	sr_mutex_lock(pipe->mutex);

	unsigned int ret = 0, pos = 0;
	while (pos < size ) {
		ret = sr_pipe_write(pipe, data + pos, size - pos);
		pos += ret;
		if (pos != size){
			if (__is_true(pipe->stopped)){
				sr_mutex_unlock(pipe->mutex);
				return pos;
			}
			if ((pipe->len - pipe->writer + pipe->reader) == 0){
				__sr_atom_add(pipe->write_waiting, 1);
				if (ret > 0 && pipe->read_waiting > 0){
					sr_mutex_broadcast(pipe->mutex);
				}
				sr_mutex_wait(pipe->mutex);
				__sr_atom_sub(pipe->write_waiting, 1);
			}
		}
	}

	if (pipe->read_waiting){
		sr_mutex_broadcast(pipe->mutex);
	}

	sr_mutex_unlock(pipe->mutex);

	return pos;
}


unsigned int sr_pipe_block_read(sr_pipe_t *pipe, char *buf, unsigned int size)
{
	if (pipe == NULL || buf == NULL || size == 0){
        LOGE("Invalid parameter: pipe=%p buf=%p size=%u\n", pipe, buf, size);
		return 0;
	}

	sr_mutex_lock(pipe->mutex);

	unsigned int pos = 0;
	for(unsigned int ret = 0; pos < size; ){
		ret = sr_pipe_read(pipe, buf + pos, size - pos);
		pos += ret;
		if (pos != size){
			if (__is_true(pipe->stopped)){
				sr_mutex_unlock(pipe->mutex);
				return pos;
			}
			if ((pipe->writer - pipe->reader) == 0){
				__sr_atom_add(pipe->read_waiting, 1);
				if (ret > 0 && pipe->write_waiting > 0){
					sr_mutex_broadcast(pipe->mutex);
				}
				sr_mutex_wait(pipe->mutex);
				__sr_atom_sub(pipe->read_waiting, 1);
			}
		}
	}

	if (pipe->write_waiting){
		sr_mutex_broadcast(pipe->mutex);
	}

	sr_mutex_unlock(pipe->mutex);

	return pos;
}


///////////////////////////////////////////////////////////////
////缓冲区
///////////////////////////////////////////////////////////////


typedef struct sr_buffer_node_t{
    sr_node_t node;
    sr_buffer_data_t buffer;
    sr_buffer_pool_t *pool;
}sr_buffer_node_t;

struct sr_buffer_pool {
    size_t align;
    size_t head_size;
    size_t data_size;
    size_t buffer_count;
    size_t max_buffer_count;
    bool destroyed;
    sr_queue_t *queue;
    char *name;
};

static void release_buffer_node(sr_node_t *node){
    sr_buffer_node_t *buffer_node = (sr_buffer_node_t*)node;
    free(buffer_node->buffer.head);
    free(buffer_node);
}

sr_buffer_pool_t* sr_buffer_pool_create(
        size_t buffer_size,
        size_t buffer_count,
        size_t max_buffer_count,
        size_t head_size,
        size_t align)
{
    sr_buffer_pool_t *pool = (sr_buffer_pool_t*) calloc(1, sizeof(sr_buffer_pool_t));
    pool->data_size = buffer_size;
    pool->buffer_count = buffer_count;
    pool->max_buffer_count = max_buffer_count;
    pool->head_size = head_size;
    pool->align = align;
    pool->queue = sr_queue_create(release_buffer_node);
    for (int i = 0; i < pool->buffer_count; ++i){
        sr_buffer_node_t *node = calloc(1, sizeof(sr_buffer_node_t));
        node->buffer.data_size = pool->data_size;
        node->buffer.head_size = pool->head_size;
        if (pool->align > 0){
            node->buffer.head = (uint8_t*)aligned_alloc(pool->align, pool->data_size + pool->head_size);
        }else {
            node->buffer.head = (uint8_t*)malloc(pool->data_size + pool->head_size);
        }
        node->buffer.data = node->buffer.head + pool->head_size;
        node->pool = pool;
        __sr_queue_push_back(pool->queue, node);
    }
    return pool;
}

void sr_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool){
    if (pp_buffer_pool && *pp_buffer_pool){
        sr_buffer_pool_t *pool = *pp_buffer_pool;
        __set_true(pool->destroyed);
        if (sr_queue_length(pool->queue) == pool->buffer_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release()[%s][%lu]\n", pool->name, pool->buffer_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}

void sr_buffer_pool_set_name(sr_buffer_pool_t *pool, const char *name)
{
    assert(pool != NULL);
    pool->name = strdup(name);
}

sr_buffer_data_t* sr_buffer_pool_alloc(sr_buffer_pool_t *pool)
{
    assert(pool != NULL);
    if (sr_queue_length(pool->queue) > 0){
        sr_buffer_node_t *node;
        __sr_queue_block_pop_front(pool->queue, node);
        return &node->buffer;
    }
    if (pool->buffer_count < pool->max_buffer_count){
        __sr_atom_add(pool->buffer_count, 1);
        sr_buffer_node_t *node = calloc(1, sizeof(sr_buffer_node_t));
        if (pool->align > 0){
            node->buffer.head = (uint8_t*)aligned_alloc(pool->align, pool->data_size + pool->head_size);
        }else {
            node->buffer.head = (uint8_t*)malloc(pool->data_size + pool->head_size);
        }
        node->buffer.frame = (sr_buffer_frame_t){0};
        node->buffer.msg = (sr_buffer_message_t){0};
        node->buffer.data = node->buffer.head + pool->head_size;
        node->pool = pool;
        return &node->buffer;
    }
    sr_buffer_node_t *node;
    __sr_queue_block_pop_front(pool->queue, node);
    return &node->buffer;
}

sr_buffer_data_t* sr_buffer_pool_realloc(sr_buffer_data_t *buffer, size_t size)
{
    assert(buffer != NULL);
    if (buffer->head){
        free(buffer->head);
    }
    sr_buffer_node_t *node = (sr_buffer_node_t*)((char *)buffer - sizeof(sr_node_t));
    node->buffer.data_size = size;
    if (node->pool->align > 0){
        node->buffer.head = (uint8_t*)aligned_alloc(node->pool->align, buffer->data_size + buffer->head_size);
    }else {
        node->buffer.head = (uint8_t*)malloc(buffer->data_size + buffer->head_size);
    }
    node->buffer.data = node->buffer.head + buffer->head_size;
    return &node->buffer;
}

void sr_buffer_pool_recycle(sr_buffer_data_t *buffer)
{
    assert(buffer != NULL);
    sr_buffer_node_t *node = (sr_buffer_node_t*)((char *)buffer - sizeof(sr_node_t));
    node->buffer.frame = (sr_buffer_frame_t){0};
    node->buffer.msg = (sr_buffer_message_t){0};
    __sr_queue_block_push_back(node->pool->queue, node);
    if (__is_true(node->pool->destroyed)){
        sr_buffer_pool_t *pool = node->pool;
        if (sr_queue_length(pool->queue) == pool->buffer_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release_delayed()[%s][%lu]\n", pool->name, pool->buffer_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}