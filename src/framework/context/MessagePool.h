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


//using namespace std;


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
            m.key = key;
        }
        Message(int key, int event)
        {
            m.key = key;
            m.subKey = event;
        }
        Message(int key, void *ptr)
        {
            m.key = key;
            m.ptr = ptr;
        }
        Message(int key, int64_t number)
        {
            m.key = key;
            m.number = number;
        }
        Message(int key, sr_buffer_data_t *buffer)
        {
            if (buffer){
                m.key = key;
                p_buffer = buffer;
                p_reference_count = new int(1);
            }
        }
        Message(int key, unsigned char *data, size_t size, sr_buffer_data_t *buffer)
        {
            if (buffer){
                m.key = key;
                m.msgLength = size;
                p_buffer = buffer;
                if (p_buffer->data_size < m.msgLength){
                    sr_buffer_pool_realloc(p_buffer, m.msgLength << 1);
                }
                if (data){
                    memcpy(p_buffer->data, data, m.msgLength);
                }
                p_reference_count = new int(1);
            }
        }
        Message(int key, std::string str, sr_buffer_data_t *buffer)
        {
            if (buffer){
                m.key = key;
                p_buffer = buffer;
                m.msgLength = str.size() + 1;
                if (p_buffer->data_size < m.msgLength){
                    sr_buffer_pool_realloc(p_buffer, m.msgLength << 1);
                }
                memcpy(p_buffer->data, str.c_str(), m.msgLength);
                p_reference_count = new int(1);
            }
        }
        Message(const Message &pkt)
        {
            if (this != &pkt){
                this->m = pkt.m;
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
                    sr_buffer_pool_recycle(p_buffer);
                    delete p_reference_count;
                }
            }
        }
        const Message& operator =(const Message& pkt)
        {
            if (this != &pkt){
                if (__sr_atom_sub(*p_reference_count, 1) == 0){
                    if (p_buffer){
                        sr_buffer_pool_recycle(p_buffer);
                        delete p_reference_count;
                    }
                }
                this->m = pkt.m;
                this->frame = pkt.frame;
                this->p_buffer = pkt.p_buffer;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
            return *this;
        }

    public:
        void SetKey(int key){
            m.key = key;
        }
        int GetKey(){
            return m.key;
        }
        void SetSubKey(int event){
            m.subKey = event;
        }
        int GetSubKey(){
            return m.subKey;
        }
        void SetPtr(void *ptr){
            m.ptr = ptr;
        }
        void* GetPtr(){
            return m.ptr;
        }
        size_t GetMsgLength(){
            return m.msgLength;
        }
        int64_t GetNumber(){
            return m.number;
        }
        std::string GetString(){
            if (p_buffer && p_buffer->data){
                return std::string((char*)p_buffer->data, m.msgLength);
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
        sr_buffer_data_t* GetBufferPtr(){
            if (p_buffer){
                return p_buffer;
            }
            return nullptr;
        }

    public:
        struct {
            int key;
            int subKey;
            size_t msgLength;
            union {
                void *ptr;
                int64_t number;
            };
        }m = {0};
        sr_buffer_frame_t frame = {0};

    private:
        sr_buffer_data_t *p_buffer = nullptr;
        int *p_reference_count = &global_reference_count;
    };




    class MessagePool {
    public:
        MessagePool(size_t messageLength,
                size_t messageCount,
                size_t maxMessageCount,
                size_t headSize,
                size_t align,
                std::string name)
        {
            p_pool = sr_buffer_pool_create(messageLength, messageCount, maxMessageCount, headSize, align);
            assert(p_pool);
            sr_buffer_pool_set_name(p_pool, name.c_str());
        }
        ~MessagePool()
        {
            sr_buffer_pool_release(&p_pool);
        }
        Message NewFrameMessage(int key)
        {
            return Message(key, sr_buffer_pool_alloc(p_pool));
        }
        Message NewStringMessage(int key, std::string str)
        {
            return Message(key, str, sr_buffer_pool_alloc(p_pool));
        }
        Message NewDataMessage(int key, unsigned char *data, size_t size)
        {
            return Message(key, data, size, sr_buffer_pool_alloc(p_pool));
        }

    private:
        sr_buffer_pool_t *p_pool = nullptr;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
