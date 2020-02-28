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

    enum {
        PktMsgType_Unknown = -1,
        PktMsgType_None = 0,
        PktMsgType_Ptr,
        PktMsgType_String,
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
            msg.type = PktMsgType_None;
        }
        SmartPkt(int _key, void *_ptr) : SmartPkt()
        {
            msg.key = _key;
            msg.ptr = _ptr;
            msg.type = PktMsgType_Ptr;
        }
        SmartPkt (sr_buffer_data_t *_buffer) : SmartPkt()
        {
            buffer = _buffer;
        }
        SmartPkt(int _key, std::string str, sr_buffer_data_t *_buffer) : SmartPkt()
        {
            buffer = _buffer;
            msg.key = _key;
            msg.size = str.length();
            msg.type = PktMsgType_String;
            if (buffer->data_size < msg.size){
                free(buffer->head);
                buffer->data_size = msg.size << 2;
                buffer->data = buffer->head = (unsigned char*)malloc(buffer->data_size);
            }
            memcpy(buffer->data, str.c_str(), msg.size + 1);
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
        void SetKey(int key){
            msg.key = key;
        }
        int GetKey(){
            return msg.key;
        }
        int GetType(){
            return msg.type;
        }
        void* GetPtr(){
            return msg.ptr;
        }
        int64_t GetNumber(){
            return msg.number;
        }
        std::string GetString(){
            return std::string((char*)buffer->data, msg.size);
        }
        unsigned char* GetHeadPtr(){
            return buffer->head;
        }
        unsigned char* GetDataPtr(){
            return buffer->data;
        }
        size_t GetDataSize(){
            return buffer->data_size;
        }

    public:
        sr_buffer_frame_t frame;

    private:
        int *reference_count;
        struct {
            int key;
            int type;
            union {
                void *ptr;
                size_t size;
                int64_t number;
            };
        }msg;
        sr_buffer_data_t *buffer;
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
        SmartPkt GetPkt(int key, std::string str)
        {
            return SmartPkt(key, str, sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
