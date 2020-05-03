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
# include <resampler/libyuv_resampler.h>

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

    static int static_reference_count = -(INT32_MAX);
    static sr_message_t static_buffer_data = {0};



    class Message {
    public:
        Message()
        {
            p_message = &static_buffer_data;
            p_reference_count = &static_reference_count;
        }
        ~Message()
        {
            if (__sr_atom_sub(*p_reference_count, 1) == 0){
                if (p_message != &static_buffer_data){
                    p_message->recycle(p_message);
//                    sr_buffer_pool_recycle(p_buffer);
                    delete p_reference_count;
                }
            }
        }
        Message(const Message &msg)
        {
            if (this != &msg){
                this->p_message = msg.p_message;
                this->p_reference_count = msg.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
        }
        const Message& operator =(const Message& pkt)
        {
            if (this != &pkt){
                if (__sr_atom_sub(*p_reference_count, 1) == 0){
                    if (p_message){
//                        sr_buffer_pool_recycle(p_buffer);
                        p_message->recycle(p_message);
                        delete p_reference_count;
                    }
                }
                this->p_message = pkt.p_message;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
            return *this;
        }

    private:
        friend class MessagePool;
        Message(int key, sr_message_t *buffer)
        {
            if (buffer){
                p_message = buffer;
                p_message->data.key = key;
                p_reference_count = new int(1);
            }
        }
        Message(int key, int event, sr_message_t *buffer)
        {
            if (buffer){
                p_message = buffer;
                p_message->data.key = key;
                p_message->data.event = event;
                p_reference_count = new int(1);
            }
        }
        Message(int key, void *object, sr_message_t *buffer)
        {
            if (buffer){
                p_message = buffer;
                p_message->data.key = key;
                p_message->data.object_ptr = object;
                p_reference_count = new int(1);
            }
        }
        Message(int key, std::string str, sr_message_t *buffer)
        {
            if (buffer){
                p_message = buffer;
                p_message->data.key = key;
                p_message->data.data_size = str.size() + 1;
                if (p_message->buffer.data_size < p_message->data.data_size){
                    sr_buffer_pool_realloc(p_message, p_message->data.data_size << 1);
                }
                memcpy(p_message->buffer.data_ptr, str.c_str(), p_message->data.data_size);
                p_reference_count = new int(1);
            }
        }
        Message(int key, unsigned char *data, size_t size, sr_message_t *buffer)
        {
            if (buffer){
                p_message = buffer;
                p_message->data.key = key;
                p_message->data.data_size = size;
                if (p_message->buffer.data_size < p_message->data.data_size){
                    sr_buffer_pool_realloc(p_message, p_message->data.data_size << 1);
                }
                if (data){
                    memcpy(p_message->buffer.data_ptr, data, p_message->data.data_size);
                }
                p_reference_count = new int(1);
            }
        }

    public:
        int key(){
            return p_message->data.key;
        }
        int event(){
            return p_message->data.event;
        }
        int64_t GetNumber(){
            return p_message->data.number;
        }
        size_t GetDataSize(){
            return p_message->data.data_size;
        }
        unsigned char* GetDataPtr(){
            return p_message->data.data_ptr;
        }
        void* GetObjectPtr(){
            return p_message->data.object_ptr;
        }
        size_t GetHeadSize(){
            return p_message->buffer.head_size;
        }
        size_t GetBufferSize(){
            return p_message->buffer.data_size;
        }
        unsigned char* GetHeadPtr(){
            return p_message->buffer.head_ptr;
        }
        unsigned char* GetBufferPtr(){
            return p_message->buffer.data_ptr;
        }
        sr_message_frame_t* GetFramePtr(){
            return &p_message->frame;
        }
        sr_message_data_t* GetMessagePtr(){
            return &p_message->data;
        }
        std::string GetString(){
            if (p_message->buffer.data_ptr && p_message->data.data_size){
                return std::string((char*)p_message->buffer.data_ptr, p_message->data.data_size);
            }
            return std::string();
        }

    private:
        int *p_reference_count = nullptr;
        sr_message_t *p_message = nullptr;
    };




    class MessagePool {
    public:
        MessagePool(std::string name,
                    size_t maxMessageLength,
                    size_t allocateMessageCount,
                    size_t maxAllocateMessageCount)
        {
            p_pool = sr_buffer_pool_create(maxMessageLength, allocateMessageCount, maxAllocateMessageCount, 0, 0);
            sr_buffer_pool_set_name(p_pool, name.c_str());
        }
        MessagePool(std::string name,
                size_t maxMessageLength,
                size_t allocateMessageCount,
                size_t maxAllocateMessageCount,
                size_t headSize,
                size_t align)
        {
            p_pool = sr_buffer_pool_create(maxMessageLength, allocateMessageCount, maxAllocateMessageCount, headSize, align);
            sr_buffer_pool_set_name(p_pool, name.c_str());
        }
        ~MessagePool()
        {
            sr_buffer_pool_release(&p_pool);
        }
        Message NewMessage(int key)
        {
            return Message(key, sr_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, int event)
        {
            return Message(key, event, sr_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, void *object)
        {
            return Message(key, object, sr_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, std::string str)
        {
            return Message(key, str, sr_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, unsigned char *data, size_t size)
        {
            return Message(key, data, size, sr_buffer_pool_alloc(p_pool));
        }

    private:
        sr_buffer_pool_t *p_pool = nullptr;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
