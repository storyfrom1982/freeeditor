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
        MsgKey_Error = -1,
        MsgKey_Exit = 0,
        MsgKey_Open = 1,
        MsgKey_Start = 2,
        MsgKey_Stop = 3,
        MsgKey_Close = 4,
        MsgKey_ProcessData = 5,
        MsgKey_ProcessEvent = 6,
        MsgKey_ProcessControl = 10,
    };


    enum {
        PktFlag_PFrame = 0,
        PktFlag_KeyFrame = 1,
    };

    static int global_reference_count = -(INT32_MAX);

    class Message {

    public:
        Message(int key = 0)
        {
            msg.key = key;
        }
        Message(int key, int event)
        {
            msg.key = key;
            msg.event = event;
        }
        Message(int key, void *ptr)
        {
            msg.key = key;
            msg.ptr = ptr;
        }
        Message(int key, int64_t number)
        {
            msg.key = key;
            msg.number = number;
        }
        Message (int key, sr_buffer_data_t *buffer)
        {
            if (buffer){
                msg.key = key;
                p_buffer = buffer;
                p_reference_count = new int(1);
            }
        }
        Message(int key, unsigned char *data, size_t size, sr_buffer_data_t *buffer)
        {
            if (buffer){
                msg.key = key;
                msg.size = size;
                p_buffer = buffer;
                if (p_buffer->data_size < msg.size){
                    free(p_buffer->head);
                    p_buffer->data_size = msg.size << 2;
                    p_buffer->data = p_buffer->head = (unsigned char*)malloc(p_buffer->data_size);
                }
                if (data){
                    memcpy(p_buffer->data, data, msg.size);
                }
                p_reference_count = new int(1);
            }
        }
        Message(int key, std::string str, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                msg.key = key;
                msg.size = str.size() + 1;
                if (p_buffer->data_size < msg.size){
                    free(p_buffer->head);
                    p_buffer->data_size = msg.size << 2;
                    p_buffer->data = p_buffer->head = (unsigned char*)malloc(p_buffer->data_size);
                }
                memcpy(p_buffer->data, str.c_str(), msg.size);
                p_reference_count = new int(1);
            }
        }
        Message(const Message &pkt)
        {
            if (this != &pkt){
                this->msg = pkt.msg;
                this->frame = pkt.frame;
                this->p_buffer = pkt.p_buffer;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
        }
        ~Message()
        {
            if (__sr_atom_sub(*p_reference_count, 1) == 0){
                if (p_buffer){
                    sr_buffer_pool_put(p_buffer);
                    delete p_reference_count;
                }
            }
        }
        const Message& operator =(const Message& pkt)
        {
            if (this != &pkt){
                if (__sr_atom_sub(*p_reference_count, 1) == 0){
                    if (p_buffer){
                        sr_buffer_pool_put(p_buffer);
                        delete p_reference_count;
                    }
                }
                this->msg = pkt.msg;
                this->frame = pkt.frame;
                this->p_buffer = pkt.p_buffer;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
            return *this;
        }

    public:
        void SetKey(int key){
            msg.key = key;
        }
        void SetPtr(void *ptr){
            msg.ptr = ptr;
        }
        void SetEvent(int event){
            msg.event = event;
        }
        int GetKey(){
            return msg.key;
        }
        int GetEvent(){
            return msg.event;
        }
        void* GetPtr(){
            return msg.ptr;
        }
        int64_t GetNumber(){
            return msg.number;
        }
        std::string GetString(){
            if (p_buffer && p_buffer->data){
                return std::string((char*)p_buffer->data, msg.size);
            }
            return std::string();
        }
        size_t GetDataSize(){
            if (p_buffer){
                return p_buffer->data_size;
            }
            return 0;
        }
        unsigned char* GetDataPtr(){
            if (p_buffer){
                return p_buffer->data;
            }
            return nullptr;
        }
        unsigned char* GetHeadPtr(){
            if (p_buffer){
                return p_buffer->head;
            }
            return nullptr;
        }

    public:
        struct {
            int key;
            int event;
            size_t size;
            union {
                void *ptr;
                int64_t number;
            };
        }msg = {0};
        sr_buffer_frame_t frame = {0};

    private:
        sr_buffer_data_t *p_buffer = nullptr;
        int *p_reference_count = &global_reference_count;
    };

    class BufferPool {

    public:
        BufferPool(size_t buffer_count,
                size_t data_size,
                size_t buffer_max_count = 64,
                size_t head_size = 0)
        {
            p_pool = sr_buffer_pool_create(buffer_count, data_size, buffer_max_count, head_size);
            assert(p_pool);
        }
        ~BufferPool()
        {
            LOGD("[DELETE]<BufferPool>[%s]\n", m_name.c_str());
            sr_buffer_pool_release(&p_pool);
        }
        void SetName(std::string name){
            m_name = name;
            sr_buffer_pool_set_name(p_pool, m_name.c_str());
        }
        Message NewFrameMessage(int key)
        {
            return Message(key, sr_buffer_pool_get(p_pool));
        }
        Message NewStringMessage(int key, std::string str)
        {
            return Message(key, str, sr_buffer_pool_get(p_pool));
        }

    private:
        std::string m_name;
        sr_buffer_pool_t *p_pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
