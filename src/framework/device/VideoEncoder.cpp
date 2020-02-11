//
// Created by yongge on 20-2-5.
//

#include "VideoEncoder.h"

void freee::VideoEncoder::openEncoder(std::string config) {

}

void freee::VideoEncoder::closeEncoder() {

}

sr_message_t *freee::VideoEncoder::GetBuffer() {
    if (sr_message_queue_putable(queue)){
//        LOGD("sr_message_queue_putable = %u\n", sr_message_queue_putable(queue));
        return sr_message_queue_get_buffer(queue);
    }
    return nullptr;
}

void freee::VideoEncoder::PutBuffer(SmartPtr<sr_buffer_t*> buffer) {
//    sr_buffer_pool_free(buffer.get());
//    sr_message_queue_put(queue, *msg);
//    LOGD("sr_message_queue_putable = %u\n", sr_message_queue_getable(queue));
}

freee::VideoEncoder::VideoEncoder() {
    queue = sr_message_queue_create();
}

freee::VideoEncoder::~VideoEncoder() {
    while (sr_message_queue_getable(queue)){
        sr_message_t *msg = sr_message_queue_get(queue);
        VideoPacket *packet = static_cast<VideoPacket *>(msg->ptr);
        videoPacket_Free(&packet);
    }
    sr_message_queue_release(&queue);
}

void freee::VideoEncoder::PutBuffer(sr_buffer_t *buffer) {
    sr_buffer_pool_free(buffer);
}
