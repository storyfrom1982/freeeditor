//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_SRBUFFERPOOL_H
#define ANDROID_SRBUFFERPOOL_H


#include <cstddef>

#include <MessageContext.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <sr_buffer_pool.h>

#ifdef __cplusplus
}
#endif


namespace freee {


    class SrMessage {

    public:
        SrMessage (sr_buffer_data_t *buffer = NULL)
        {
            this->buffer = buffer;
            this->frame = (sr_buffer_frame_t){0};
            reference_count = new int(1);
        }
        SrMessage(const SrMessage &msg)
        {
            if (this != &msg){
                this->buffer = msg.buffer;
                this->frame = msg.frame;
                this->reference_count = msg.reference_count;
                __sr_atom_add(*reference_count, 1);
            }
        }
        ~SrMessage()
        {
            __sr_atom_sub(*reference_count, 1);
            if ((*reference_count) == 0){
                if (buffer){
                    sr_buffer_pool_put(buffer);
                }else if (frame.js){
                    free(frame.js);
                }
                delete reference_count;
            }
        }
        const SrMessage& operator =(const SrMessage& msg)
        {
            this->~SrMessage();
            this->buffer = msg.buffer;
            this->frame = msg.frame;
            this->reference_count = msg.reference_count;
            __sr_atom_add(*reference_count, 1);
            return *this;
        }

    public:
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
        SrMessage GetBuffer()
        {
            return SrMessage(sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
