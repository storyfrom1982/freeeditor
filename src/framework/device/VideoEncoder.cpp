//
// Created by yongge on 20-2-5.
//

#include "VideoEncoder.h"
#include "X264Encoder.h"


using namespace freee;


VideoEncoder::VideoEncoder() {

}

VideoEncoder::~VideoEncoder() {

}

void VideoEncoder::OpenEncoder(json& cfg) {
    OnOpenEncoder(cfg);
}

void VideoEncoder::CloseEncoder() {
    OnCloseEncoder();
}

void VideoEncoder::EncodeVideo(sr_buffer_t *buffer) {
    OnEncodeVideo(buffer);
}

VideoEncoder *VideoEncoder::Create(std::string name) {
    return new X264Encoder;
}
