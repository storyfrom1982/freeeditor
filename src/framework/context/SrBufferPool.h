//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_SRBUFFERPOOL_H
#define ANDROID_SRBUFFERPOOL_H


#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>
#include <sr_buffer_pool.h>
#include <video_resample.h>

#ifdef __cplusplus
}
#endif


namespace freee {

    typedef struct sr_msg_t{
        int key;
        int size;
        char *js;
        void *obj;
        union {
            void *ptr;
            int64_t number;
        };
        double decimal;
    }sr_msg_t;

    class SrPkt {

    public:
        SrPkt (sr_buffer_data_t *buffer = NULL)
        {
            this->buffer = buffer;
            this->frame = (sr_buffer_frame_t){0};
            this->msg = (sr_msg_t){0};
            reference_count = new int(1);
        }
        SrPkt(const SrPkt &pkt)
        {
            if (this != &pkt){
                this->msg = pkt.msg;
                this->buffer = pkt.buffer;
                this->frame = pkt.frame;
                this->reference_count = pkt.reference_count;
                __sr_atom_add(*reference_count, 1);
            }
        }
        ~SrPkt()
        {
            __sr_atom_sub(*reference_count, 1);
            if ((*reference_count) == 0){
                if (buffer){
                    sr_buffer_pool_put(buffer);
                }else if (msg.js){
                    free(msg.js);
                }
                delete reference_count;
            }
        }
        const SrPkt& operator =(const SrPkt& pkt)
        {
            this->~SrPkt();
            this->msg = pkt.msg;
            this->frame = pkt.frame;
            this->buffer = pkt.buffer;
            this->reference_count = pkt.reference_count;
            __sr_atom_add(*reference_count, 1);
            return *this;
        }

    public:
        sr_msg_t msg;
        sr_buffer_data_t *buffer;
        sr_buffer_frame_t frame;

    private:
        int *reference_count;
    };



    class SrBufferPool {

    public:
        SrBufferPool(size_t count, size_t size)
        {
            pool = sr_buffer_pool_create(count, size);
        }
        ~SrBufferPool()
        {
            sr_buffer_pool_release(&pool);
        }
        SrPkt GetBuffer()
        {
            return SrPkt(sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
