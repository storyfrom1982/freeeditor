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
    static sr_buffer_data_t static_buffer_data = {0};



    class Message {
    public:
        Message()
        {
            p_buffer = &static_buffer_data;
            p_reference_count = &static_reference_count;
        }
        ~Message()
        {
            if (__sr_atom_sub(*p_reference_count, 1) == 0){
                if (p_buffer != &static_buffer_data){
                    sr_buffer_pool_recycle(p_buffer);
                    delete p_reference_count;
                }
            }
        }
        Message(const Message &msg)
        {
            if (this != &msg){
                this->p_buffer = msg.p_buffer;
                this->p_reference_count = msg.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
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
                this->p_buffer = pkt.p_buffer;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
            return *this;
        }

    private:
        friend class MessagePool;
        Message(int key, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                p_buffer->msg.key = key;
                p_reference_count = new int(1);
            }
        }
        Message(int key, int event, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                p_buffer->msg.key = key;
                p_buffer->msg.event = event;
                p_reference_count = new int(1);
            }
        }
        Message(int key, void *object, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                p_buffer->msg.key = key;
                p_buffer->msg.objectPtr = object;
                p_reference_count = new int(1);
            }
        }
        Message(int key, std::string str, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                p_buffer->msg.key = key;
                p_buffer->msg.length = str.size() + 1;
                if (p_buffer->data_size < p_buffer->msg.length){
                    sr_buffer_pool_realloc(p_buffer, p_buffer->msg.length << 1);
                }
                memcpy(p_buffer->data, str.c_str(), p_buffer->msg.length);
                p_reference_count = new int(1);
            }
        }
        Message(int key, unsigned char *data, size_t size, sr_buffer_data_t *buffer)
        {
            if (buffer){
                p_buffer = buffer;
                p_buffer->msg.key = key;
                p_buffer->msg.length = size;
                if (p_buffer->data_size < p_buffer->msg.length){
                    sr_buffer_pool_realloc(p_buffer, p_buffer->msg.length << 1);
                }
                if (data){
                    memcpy(p_buffer->data, data, p_buffer->msg.length);
                }
                p_reference_count = new int(1);
            }
        }

    public:
        int key(){
            return p_buffer->msg.key;
        }
        int event(){
            return p_buffer->msg.event;
        }
        size_t length(){
            return p_buffer->msg.length;
        }
        int64_t GetNumber(){
            return p_buffer->msg.number;
        }
        void* GetObjectPtr(){
            return p_buffer->msg.objectPtr;
        }
        void* GetSharePtr(){
            return p_buffer->msg.sharePtr;
        }
        size_t GetHeadSize(){
            return p_buffer->head_size;
        }
        unsigned char* GetHeadPtr(){
            return p_buffer->head;
        }
        size_t GetBufferSize(){
            return p_buffer->data_size;
        }
        unsigned char* GetBufferPtr(){
            return p_buffer->data;
        }
        sr_buffer_frame_t* GetFramePtr(){
            return &p_buffer->frame;
        }
        sr_buffer_message_t* GetMessagePtr(){
            return &p_buffer->msg;
        }
        std::string GetString(){
            if (p_buffer->data && p_buffer->msg.length){
                return std::string((char*)p_buffer->data, p_buffer->msg.length);
            }
            return std::string();
        }

    private:
        int *p_reference_count = nullptr;
        sr_buffer_data_t *p_buffer = nullptr;
    };




    class MessagePool {
    public:
        MessagePool(std::string name = "MessagePool",
                size_t maxMessageLength = 10240,
                size_t allocMessageCount = 10,
                size_t maxMessageCount = 64,
                size_t headSize = 0,
                size_t align = 0)
        {
            p_pool = sr_buffer_pool_create(maxMessageLength, allocMessageCount, maxMessageCount, headSize, align);
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
