//
// Created by yongge on 20-2-14.
//

#include <sr_buffer_frame.h>
#include "X264Encoder.h"


using namespace freee;

int X264Encoder::OnOpenEncoder(json& cfg) {
    LOGD("X264Encoder::OnOpenEncoder: %s\n", cfg.dump().c_str());
    param = (x264_param_t){0};

    x264_param_default(&param);
    x264_param_default_preset(&param, "veryfast",  "zerolatency");//animation");
    x264_param_apply_profile(&param,"high");

    param.i_csp = X264_CSP_I420;
    param.i_log_level = X264_LOG_NONE;
    param.i_width = cfg["width"];
    param.i_height = cfg["height"];

    bool vbr = cfg["vbr"];
    uint32_t fr = cfg["fps"];
    uint32_t bitrate = cfg["bitRate"];

    // if input buffer rate is reliable, use the buffer rate for calc bitrate
    if ( fr > 0){
        param.i_fps_num = fr*1000;
        param.i_fps_den=1000;
        param.b_vfr_input = 0;
    }
    else{
        // if input buffer rate is not reliable, use the i64 for calc bitrate
        param.b_vfr_input = 1;
        param.i_timebase_num = 1;
        param.i_timebase_den = 1000000;
        param.i_fps_num = 0;
    }

    // i_bitrate: kbit
    param.rc.i_bitrate = bitrate / 1000;
    param.rc.i_vbv_buffer_size= param.rc.i_bitrate ;
    param.rc.i_vbv_max_bitrate= param.rc.i_bitrate ;
    param.rc.f_rate_tolerance = 0.1f;
    param.rc.i_lookahead = 0;
    //param.rc.f_vbv_buffer_init=0.89;
    //param.rc.f_rate_tolerance = 1.0;

    if (bitrate > 0){
        if (vbr)
        {
            param.rc.i_rc_method = X264_RC_CRF;
            param.rc.f_rf_constant = 27;
            param.rc.f_rf_constant_max = 51;
        }
        else{

            param.rc.i_rc_method = X264_RC_ABR;
            param.rc.i_qp_min = 0;//10;
            param.rc.i_qp_step = 4;
            param.rc.i_qp_max = 51;//100;
        }
        //param.rc.f_ip_factor = 0.6;
    }
    else
    {
        param.rc.i_rc_method = X264_RC_CQP;
        param.rc.i_qp_constant = 23;
        param.rc.f_ip_factor = 1.4;
    }

    m_handle = x264_encoder_open(&param);

    x264_nal_t* encoded;
    int nal;

    if (x264_encoder_headers(m_handle, &encoded, &nal) < 0){
        return -1;
    }

    if (nal < 2)
        return -1;

    unsigned char *sps = encoded[0].p_payload + 4;
    unsigned char *pps = encoded[1].p_payload + 4;
    short sps_size = encoded[0].i_payload-4;
    short pps_size = encoded[1].i_payload - 4;

    m_frameId = 0;
    LOGD("X264Encoder::OnOpenEncoder: %p\n", m_handle);
    return 0;
}

void X264Encoder::OnCloseEncoder() {
    if (m_handle){
        x264_encoder_close(m_handle);
    }
}

void X264Encoder::OnEncodeVideo(SmartPkt buffer) {

//    LOGD("X264Encoder::OnOpenEncoder: enter\n");

    sr_buffer_frame_t *pkt = &buffer.frame;

    x264_picture_t pic_out;
    x264_picture_t  pic;
    x264_picture_init( &pic_out );
    x264_picture_init( &pic );

    pic.img.i_csp = X264_CSP_I420;
    for (int i=0; i<3; i++){
        pic.img.plane[i] = pkt->plane[i].data;
    }
    pic.img.i_plane = 3;
    pic.img.i_stride[0] = pkt->plane[0].stride;
    pic.img.i_stride[1] = pkt->plane[1].stride;
    pic.img.i_stride[2] = pkt->plane[2].stride;

    m_frameId ++;
    long long timeStamp = m_frameId;
    pic.i_pts = timeStamp;

    pic.i_type = X264_TYPE_AUTO;

    x264_nal_t *nal;
    int i_nal;


    int result = x264_encoder_encode(m_handle, &nal, &i_nal, &pic, &pic_out );
    if( result < 0 )
    {
        LOGD("x264_encoder_encode failed\n");
    }

    int frameLen = 0;
    for (int i=0; i<i_nal; i++){
        int nal_len = nal[i].i_payload;
        frameLen += nal_len;
    }

    long long tmStamp = param.b_vfr_input ? pic_out.i_dts :
                        pic_out.i_dts*1000000*param.i_fps_den/param.i_fps_num;

//    LOGD("X264Encoder::OnOpenEncoder: size=%d  i64=%ld\n", frameLen, tmStamp);
}

X264Encoder::~X264Encoder() {
    OnCloseEncoder();
}

