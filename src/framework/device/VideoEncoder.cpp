//
// Created by yongge on 20-2-5.
//

#include "VideoEncoder.h"


using namespace freee;


VideoEncoder::VideoEncoder() {

}

VideoEncoder::~VideoEncoder() {

}

void VideoEncoder::OpenEncoder(std::string config) {

}

void VideoEncoder::CloseEncoder() {

}

sr_buffer_t* VideoEncoder::GetBuffer() {

    return nullptr;
}

void VideoEncoder::PutBuffer(SmartPtr<sr_buffer_t*> buffer) {

}

void VideoEncoder::PutBuffer(sr_buffer_t *buffer) {
    sr_buffer_pool_free(buffer);
}
