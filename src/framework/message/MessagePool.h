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


    static sr_msg_t static_msg = {0};


    class Message {
    public:
        Message()
        {
            p_msg = &static_msg;
        }
        ~Message()
        {
            sr_msg_reference_sub(p_msg);
        }
        Message(const Message &msg)
        {
            if (this != &msg){
                this->p_msg = msg.p_msg;
                sr_msg_reference_add(p_msg);
            }
        }
        const Message& operator =(const Message& message)
        {
            if (this != &message){
                sr_msg_reference_sub(p_msg);
                this->p_msg = message.p_msg;
                sr_msg_reference_add(p_msg);
            }
            return *this;
        }

    private:
        friend class MessagePool;
        Message(int key, sr_msg_t *msg)
        {
            if (msg){
                p_msg = msg;
                p_msg->type.key = key;
                sr_msg_reference_add(p_msg);
            }
        }
        Message(int key, int event, sr_msg_t *msg)
        {
            if (msg){
                p_msg = msg;
                p_msg->type.key = key;
                p_msg->type.i32 = event;
                sr_msg_reference_add(p_msg);
            }
        }
        Message(int key, void *object, sr_msg_t *msg)
        {
            if (msg){
                p_msg = msg;
                p_msg->type.key = key;
                p_msg->type.obj = object;
                sr_msg_reference_add(p_msg);
            }
        }
        Message(int key, std::string str, sr_msg_t *msg)
        {
            if (msg){
                p_msg = msg;
                p_msg->type.key = key;
                p_msg->type.size = str.size() + 1;
                if (p_msg->buffer.data_size < p_msg->type.size){
                    p_msg->realloc(p_msg, p_msg->type.size << 1);
                }
                memcpy(p_msg->buffer.data, str.c_str(), p_msg->type.size);
                sr_msg_reference_add(p_msg);
            }
        }
        Message(int key, unsigned char *data, size_t size, sr_msg_t *msg)
        {
            if (msg){
                p_msg = msg;
                p_msg->type.key = key;
                p_msg->type.size = size;
                if (p_msg->buffer.data_size < p_msg->type.size){
                    p_msg->realloc(p_msg, p_msg->type.size << 1);
                }
                if (data){
                    memcpy(p_msg->buffer.data, data, p_msg->type.size);
                }
                sr_msg_reference_add(p_msg);
            }
        }

    public:
        int key(){
            return p_msg->type.key;
        }
        int i32(){
            return p_msg->type.i32;
        }
        int64_t i64(){
            return p_msg->type.i64;
        }
        float f32(){
            return p_msg->type.f32;
        }
        double f64(){
            return p_msg->type.f64;
        }
        size_t size(){
            return p_msg->type.size;
        }
        unsigned char* data(){
            return p_msg->type.data;
        }
        void* obj(){
            return p_msg->type.obj;
        }
        sr_msg_type_t* msgType(){
            return &p_msg->type;
        }
        sr_msg_frame_t* msgFrame(){
            return &p_msg->frame;
        }
        size_t bufferSize(){
            return p_msg->buffer.data_size;
        }
        unsigned char* bufferData(){
            return p_msg->buffer.data;
        }
        size_t bufferHeadSize(){
            return p_msg->buffer.head_size;
        }
        unsigned char* bufferHead(){
            return p_msg->buffer.head;
        }
        std::string getString(){
            if (p_msg->buffer.data && p_msg->type.size){
                return std::string((char*)p_msg->buffer.data, p_msg->type.size);
            }
            return std::string();
        }

    private:
        sr_msg_t *p_msg = nullptr;
    };




    class MessagePool {
    public:
        MessagePool(std::string name,
                    size_t maxMessageLength,
                    size_t allocateMessageCount,
                    size_t maxAllocateMessageCount)
        {
            p_pool = sr_msg_buffer_pool_create(name.c_str(),
                                               allocateMessageCount,
                                               maxAllocateMessageCount,
                                               maxMessageLength, 0, 0);
        }
        MessagePool(std::string name,
                size_t maxMessageLength,
                size_t allocateMessageCount,
                size_t maxAllocateMessageCount,
                size_t headSize,
                size_t align)
        {
            p_pool = sr_msg_buffer_pool_create(name.c_str(),
                                               allocateMessageCount,
                                               maxAllocateMessageCount,
                                               maxMessageLength,
                                               headSize,
                                               align);
        }
        ~MessagePool()
        {
            sr_msg_buffer_pool_release(&p_pool);
        }
        Message NewMessage(int key)
        {
            return Message(key, sr_msg_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, int event)
        {
            return Message(key, event, sr_msg_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, void *object)
        {
            return Message(key, object, sr_msg_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, std::string str)
        {
            return Message(key, str, sr_msg_buffer_pool_alloc(p_pool));
        }
        Message NewMessage(int key, unsigned char *data, size_t size)
        {
            return Message(key, data, size, sr_msg_buffer_pool_alloc(p_pool));
        }

    private:
        sr_msg_buffer_pool_t *p_pool = nullptr;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
