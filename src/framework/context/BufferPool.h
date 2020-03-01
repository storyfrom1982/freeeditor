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
        PktMsgOpen = 1,
        PktMsgStart = 2,
        PktMsgStop = 3,
        PktMsgClose = 4,
        PktMsgProcessMedia = 5,
        PktMsgControl = 6,
        PktMsgReserved = 10,
    };

    class SmartPkt {

    public:
        SmartPkt() : frame({0}), m_msg({0}), p_buffer(nullptr)
        {
            p_reference_count = new int(1);
        }
        SmartPkt(int key) : SmartPkt()
        {
            m_msg.key = key;
        }
        SmartPkt(int key, void *ptr) : SmartPkt()
        {
            m_msg.key = key;
            m_msg.ptr = ptr;
        }
        SmartPkt (int key, sr_buffer_data_t *buffer) : SmartPkt()
        {
            m_msg.key = key;
            p_buffer = buffer;
        }
        SmartPkt(int key, std::string str, sr_buffer_data_t *buffer) : SmartPkt()
        {
            if (buffer){
                p_buffer = buffer;
                m_msg.key = key;
                m_msg.size = str.length();
                if (p_buffer->data_size < m_msg.size){
                    free(p_buffer->head);
                    p_buffer->data_size = m_msg.size << 2;
                    p_buffer->data = p_buffer->head = (unsigned char*)malloc(p_buffer->data_size);
                }
                memcpy(p_buffer->data, str.c_str(), m_msg.size + 1);
            }
        }
        SmartPkt(const SmartPkt &pkt)
        {
            if (this != &pkt){
                this->frame = pkt.frame;
                this->m_msg = pkt.m_msg;
                this->p_buffer = pkt.p_buffer;
                this->p_reference_count = pkt.p_reference_count;
                __sr_atom_add(*p_reference_count, 1);
            }
        }
        ~SmartPkt()
        {
            if (__sr_atom_sub(*p_reference_count, 1) == 0){
                if (p_buffer){
                    sr_buffer_pool_put(p_buffer);
                }
                delete p_reference_count;
            }
        }
        const SmartPkt& operator =(const SmartPkt& pkt)
        {
            this->~SmartPkt();
            this->frame = pkt.frame;
            this->m_msg = pkt.m_msg;
            this->p_buffer = pkt.p_buffer;
            this->p_reference_count = pkt.p_reference_count;
            __sr_atom_add(*p_reference_count, 1);
            return *this;
        }

    public:
        void SetKey(int key){
            m_msg.key = key;
        }
        int GetKey(){
            return m_msg.key;
        }
        void* GetPtr(){
            return m_msg.ptr;
        }
        int64_t GetNumber(){
            return m_msg.number;
        }
        std::string GetString(){
            if (p_buffer && p_buffer->data){
                return std::string((char*)p_buffer->data, m_msg.size);
            }
            return std::string();
        }
        unsigned char* GetHeadPtr(){
            if (p_buffer){
                return p_buffer->head;
            }
            return nullptr;
        }
        unsigned char* GetDataPtr(){
            if (p_buffer){
                return p_buffer->data;
            }
            return nullptr;
        }
        size_t GetDataSize(){
            if (p_buffer){
                return p_buffer->data_size;
            }
            return 0;
        }

    public:
        sr_buffer_frame_t frame;

    private:
        struct {
            int key;
            union {
                void *ptr;
                size_t size;
                int64_t number;
            };
        }m_msg;

        int *p_reference_count;
        sr_buffer_data_t *p_buffer;
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
        SmartPkt GetPkt(int key)
        {
            return SmartPkt(key, sr_buffer_pool_get(p_pool));
        }
        SmartPkt GetPkt(int key, std::string str)
        {
            return SmartPkt(key, str, sr_buffer_pool_get(p_pool));
        }

    private:
        std::string m_name;
        sr_buffer_pool_t *p_pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
