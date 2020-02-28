//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_SRBUFFERPOOL_H
#define ANDROID_SRBUFFERPOOL_H

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>
# include <sr_buffer_frame.h>

#ifdef __cplusplus
}
#endif


namespace freee {

    enum {
        PktMsgError = -1,
        PktMsgExit = 0,
    };

    class SmartPkt {

    public:
        SmartPkt() : msg({0}), frame({0}), buffer(nullptr)
        {
            reference_count = new int(1);
        }
        SmartPkt(int _key) : SmartPkt()
        {
            msg.key = _key;
        }
        SmartPkt(int _key, void *_ptr) : SmartPkt()
        {
            msg.key = _key;
            msg.ptr = _ptr;
        }
        SmartPkt(int _key, const char *data, int size) : SmartPkt()
        {
            msg.key = _key;
            msg.size = size;
            msg.json = strndup(data, size);
        }
        SmartPkt (sr_buffer_data_t *_buffer) : SmartPkt()
        {
            buffer = _buffer;
        }
        SmartPkt(const SmartPkt &pkt)
        {
            if (this != &pkt){
                this->msg = pkt.msg;
                this->buffer = pkt.buffer;
                this->frame = pkt.frame;
                this->reference_count = pkt.reference_count;
                __sr_atom_add(*reference_count, 1);
            }
        }
        ~SmartPkt()
        {
            __sr_atom_sub(*reference_count, 1);
            if ((*reference_count) == 0){
                if (buffer){
                    sr_buffer_pool_put(buffer);
                }
                if (msg.json){
                    free(msg.json);
                    msg.json = nullptr;
                }
                delete reference_count;
            }
        }
        const SmartPkt& operator =(const SmartPkt& pkt)
        {
            this->~SmartPkt();
            this->msg = pkt.msg;
            this->frame = pkt.frame;
            this->buffer = pkt.buffer;
            this->reference_count = pkt.reference_count;
            __sr_atom_add(*reference_count, 1);
            return *this;
        }

    public:
        struct {
            int key;
            int size;
            char *json;
            union {
                void *ptr;
                int64_t number;
            };
            double decimal;
            void *troubledPtr;
        }msg;

        sr_buffer_data_t *buffer;
        sr_buffer_frame_t frame;

    private:
        int *reference_count;
    };



    class BufferPool {

    public:
        BufferPool(size_t buffer_count, size_t data_size, size_t head_size = 0)
        {
            pool = sr_buffer_pool_create(buffer_count, data_size, head_size);
            assert(pool);
        }
        ~BufferPool()
        {
            sr_buffer_pool_release(&pool);
        }
        SmartPkt GetPkt()
        {
            return SmartPkt(sr_buffer_pool_get(pool));
        }
        SmartPkt AllocPkt()
        {
            return SmartPkt(sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
