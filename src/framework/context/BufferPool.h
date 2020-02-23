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
# include <video_resample.h>

#ifdef __cplusplus
}
#endif


namespace freee {

    class SmartMsg {
    public:
        SmartMsg(int _key = 0) :
        key(_key),
        size(0),
        number(0),
        decimal(0),
        json(""),
        obj(nullptr){}

        SmartMsg(int _key, int64_t _number) : SmartMsg(_key){
            number = _number;
        }
        SmartMsg(int _key, double _decimal) : SmartMsg(_key){
            decimal = _decimal;
        }
        SmartMsg(int _key, void *_ptr) : SmartMsg(_key){
            ptr = _ptr;
        }
        SmartMsg(int _key, std::string _json) : SmartMsg(_key){
            json = _json;
        }
        void SetTroubledPtr(void *ptr){
            obj = ptr;
        }

    public:
        int key;
        size_t size;
        union {
            void *ptr;
            int64_t number;
        };
        double decimal;
        void *obj;
        std::string json;
    };

    class SmartPkt {

    public:
        SmartPkt (int key = 0) : msg(key)
        {
            this->buffer = nullptr;
            this->frame = (sr_buffer_frame_t){0};
            reference_count = new int(1);
        }
        SmartPkt (sr_buffer_data_t *buffer) : msg(0)
        {
            this->buffer = buffer;
            this->frame = (sr_buffer_frame_t){0};
            reference_count = new int(1);
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
//                if (msg.json){
//                    free(msg.json);
//                }
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
        sr_buffer_data_t *buffer;
        sr_buffer_frame_t frame;
        SmartMsg msg;
//        struct message {
//            int key;
//            int size;
//            char *json;
//            void *obj;
//            union {
//                void *ptr;
//                int64_t number;
//            };
//            double decimal;
//        }msg;

    private:
        int *reference_count;
    };



    class BufferPool {

    public:
        BufferPool(size_t count, size_t size)
        {
            pool = sr_buffer_pool_create(count, size);
        }
        ~BufferPool()
        {
            sr_buffer_pool_release(&pool);
        }
        SmartPkt GetPkt()
        {
            return SmartPkt(sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
