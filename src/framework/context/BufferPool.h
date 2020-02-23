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
                number(0),
                decimal(0),
                json(""),
                troubledPtr(nullptr){}

        SmartMsg(int _key, int64_t _number, double _decimal, std::string _json) :
                key(_key),
                number(_number),
                decimal(_decimal),
                json(_json),
                troubledPtr(nullptr){}

        SmartMsg(int _key, void *_ptr) : SmartMsg(_key){
            ptr = _ptr;
        }
        SmartMsg(int _key, int64_t _number) : SmartMsg(_key){
            number = _number;
        }
        SmartMsg(int _key, double _decimal) : SmartMsg(_key){
            decimal = _decimal;
        }
        SmartMsg(int _key, std::string _json) : SmartMsg(_key){
            json = _json;
        }
//        void SetKey(int _key){
//            key = _key;
//        }
        void SetTroubledPtr(void *ptr){
            troubledPtr = ptr;
        }

    public:
        int GetKey(){
            return key;
        }
        void* GetPtr(){
            return ptr;
        }
        int64_t GetNumber(){
            return number;
        }
        double GetDecimal(){
            return decimal;
        }
        std::string& GetJson(){
            return json;
        }
        void* GetTroubledPtr(){
            return troubledPtr;
        }

    private:
        int key;
        union {
            void *ptr;
            int64_t number;
        };
        double decimal;
        void *troubledPtr;
        std::string json;
    };

    class SmartPkt {

    public:
//        SmartPkt (int key = 0) : msg(key)
//        {
//            this->buffer = nullptr;
//            this->frame = (sr_buffer_frame_t){0};
//            reference_count = new int(1);
//        }
        SmartPkt (SmartMsg _msg) : msg(_msg)
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
//            void *troubledPtr;
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
