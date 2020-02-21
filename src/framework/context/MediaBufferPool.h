//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_SRBUFFERPOOL_H
#define ANDROID_SRBUFFERPOOL_H

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

    class MediaPacket {

    public:
        MediaPacket (int key = 0)
        {
            this->buffer = nullptr;
            this->frame = (sr_buffer_frame_t){0};
            this->msg = (struct message){0};
            this->msg.key = key;
            reference_count = new int(1);
        }
        MediaPacket (sr_buffer_data_t *buffer)
        {
            this->buffer = buffer;
            this->frame = (sr_buffer_frame_t){0};
            this->msg = (struct message){0};
            reference_count = new int(1);
        }
        MediaPacket(const MediaPacket &pkt)
        {
            if (this != &pkt){
                this->msg = pkt.msg;
                this->buffer = pkt.buffer;
                this->frame = pkt.frame;
                this->reference_count = pkt.reference_count;
                __sr_atom_add(*reference_count, 1);
            }
        }
        ~MediaPacket()
        {
            __sr_atom_sub(*reference_count, 1);
            if ((*reference_count) == 0){
                if (buffer){
                    sr_buffer_pool_put(buffer);
                }
                if (msg.json){
                    free(msg.json);
                }
                delete reference_count;
            }
        }
        const MediaPacket& operator =(const MediaPacket& pkt)
        {
            this->~MediaPacket();
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

        struct message {
            int key;
            int size;
            char *json;
            void *obj;
            union {
                void *ptr;
                int64_t number;
            };
            double decimal;
        }msg;

    private:
        int *reference_count;
    };



    class MediaBufferPool {

    public:
        MediaBufferPool(size_t count, size_t size)
        {
            pool = sr_buffer_pool_create(count, size);
        }
        ~MediaBufferPool()
        {
            sr_buffer_pool_release(&pool);
        }
        MediaPacket GetBuffer()
        {
            return MediaPacket(sr_buffer_pool_get(pool));
        }

    private:
        sr_buffer_pool_t *pool;
    };


}



#endif //ANDROID_SRBUFFERPOOL_H
