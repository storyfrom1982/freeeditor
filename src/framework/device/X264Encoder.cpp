//
// Created by yongge on 20-2-14.
//

#include "X264Encoder.h"


using namespace freee;

int X264Encoder::OnOpenEncoder(json& cfg) {
    LOGD("X264Encoder::OnOpenEncoder: %s\n", cfg.dump().c_str());
    m_param = (x264_param_t){0};
    m_handle = x264_encoder_open(&m_param);
    LOGD("X264Encoder::OnOpenEncoder: %p\n", m_handle);
    return 0;
}

void X264Encoder::OnCloseEncoder() {

}

void X264Encoder::OnEncodeVideo(sr_buffer_t *buffer) {
    sr_buffer_pool_free(buffer);
}
