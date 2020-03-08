//
// Created by yongge on 20-2-14.
//

#include <sr_buffer_frame.h>
#include <bitstream.h>
#include "X264VideoEncoder.h"


using namespace freee;


X264VideoEncoder::X264VideoEncoder() {
    m_handle = nullptr;
}

X264VideoEncoder::~X264VideoEncoder() {

}


int X264VideoEncoder::OpenModule() {
    LOGD("X264VideoEncoder::OnOpenEncoder: %s\n", m_config.dump().c_str());
    m_param = (x264_param_t){0};

    x264_param_default(&m_param);
    x264_param_default_preset(&m_param, "veryfast",  "zerolatency");//animation");
    x264_param_apply_profile(&m_param,"high");

    m_param.i_csp = X264_CSP_I420;
    m_param.i_log_level = X264_LOG_NONE;
    m_param.i_width = m_config["codecWidth"];
    m_param.i_height = m_config["codecHeight"];

    bool vbr = m_config["codecVBR"];
    uint32_t fr = m_config["codecFPS"];
    uint32_t bitrate = m_config["codecBitRate"];

    // if input buffer rate is reliable, use the buffer rate for calc bitrate
    if ( fr > 0){
        m_param.i_fps_num = fr*1000;
        m_param.i_fps_den=1000;
        m_param.b_vfr_input = 0;
    }
    else{
        // if input buffer rate is not reliable, use the i64 for calc bitrate
        m_param.b_vfr_input = 1;
        m_param.i_timebase_num = 1;
        m_param.i_timebase_den = 1000000;
        m_param.i_fps_num = 0;
    }

    // i_bitrate: kbit
    m_param.rc.i_bitrate = bitrate / 1;
    m_param.rc.i_vbv_buffer_size= m_param.rc.i_bitrate ;
    m_param.rc.i_vbv_max_bitrate= m_param.rc.i_bitrate ;
    m_param.rc.f_rate_tolerance = 0.1f;
    m_param.rc.i_lookahead = 0;
    //param.rc.f_vbv_buffer_init=0.89;
    //param.rc.f_rate_tolerance = 1.0;

    if (bitrate > 0){
        if (vbr)
        {
            m_param.rc.i_rc_method = X264_RC_CRF;
            m_param.rc.f_rf_constant = 27;
            m_param.rc.f_rf_constant_max = 51;
        }
        else{

            m_param.rc.i_rc_method = X264_RC_ABR;
            m_param.rc.i_qp_min = 0;//10;
            m_param.rc.i_qp_step = 4;
            m_param.rc.i_qp_max = 51;//100;
        }
        //param.rc.f_ip_factor = 0.6;
    }
    else
    {
        m_param.rc.i_rc_method = X264_RC_CQP;
        m_param.rc.i_qp_constant = 23;
        m_param.rc.f_ip_factor = 1.4;
    }

    m_handle = x264_encoder_open(&m_param);

//    m_extraConfig = GenAvc1();
    m_extraConfig = GenH264Config();

    LOGD("X264VideoEncoder::OnOpenEncoder: %p\n", m_handle);
    return 0;
}

void X264VideoEncoder::CloseModule() {
    if (m_handle){
        x264_encoder_close(m_handle);
        m_handle = nullptr;
    }
}

int X264VideoEncoder::ProcessMediaByModule(SmartPkt pkt) {
    //    LOGD("X264VideoEncoder::OnOpenEncoder: enter\n");

    sr_buffer_frame_t *frame = &(pkt.frame);

    x264_picture_t pic_out;
    x264_picture_t  pic;
    x264_picture_init( &pic_out );
    x264_picture_init( &pic );

    pic.img.i_csp = X264_CSP_I420;
    for (int i=0; i<3; i++){
        pic.img.plane[i] = frame->plane[i].data;
    }
    pic.img.i_plane = 3;
    pic.img.i_stride[0] = frame->plane[0].stride;
    pic.img.i_stride[1] = frame->plane[1].stride;
    pic.img.i_stride[2] = frame->plane[2].stride;

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

    SmartPkt opkt = p_bufferPool->GetPkt(PktMsgProcessMedia);
    opkt.SetPtr(this);
    opkt.frame.flag = PktFlag_PFrame;

    uint8_t *dst = opkt.GetDataPtr();
    int offset = 0;

    x264_nal_t *p_spspps;
    int i_spspps;
    // Translate int to enum
    switch (pic_out.i_type) {
        case X264_TYPE_IDR:
//            x264_encoder_headers(m_handle, &p_spspps, &i_spspps);
//            LOGD("x264_encoder_encode nal idr frame count %d\n", i_nal);
//            memcpy(dst + offset, m_extraConfig.c_str(), m_extraConfig.size());
//            offset += m_extraConfig.size();

//            memcpy(dst + offset, p_spspps[0].p_payload, p_spspps[0].i_payload);
//            offset += p_spspps[0].i_payload;
//            memcpy(dst + offset, p_spspps[1].p_payload, p_spspps[1].i_payload);
//            offset += p_spspps[1].i_payload;
            opkt.frame.flag = PktFlag_KeyFrame;
            break;
        case X264_TYPE_I:
//            LOGD("x264_encoder_encode nal iframe count %d\n", i_nal);
            break;
        case X264_TYPE_P:
//            LOGD("x264_encoder_encode nal pframe count %d\n", i_nal);
            break;

        case X264_TYPE_B:
        case X264_TYPE_BREF:
//            LOGD("x264_encoder_encode nal bframe count %d\n", i_nal);
            break;
        default:
            // The API is defined as returning a type.
            break;
    }

    for (int i = 0; i < i_nal; i++){
        int nal_len = nal[i].i_payload;
        memcpy(&dst[offset], nal[i].p_payload, nal_len);
        offset += nal_len;
    }

//    for (int i = 0; i < i_nal; i++){
//        int nal_len = nal[i].i_payload;
//        memcpy(&dst[offset + 4], nal[i].p_payload + 4, nal_len - 4);
//        dst[offset + 0] = 0;
//        dst[offset + 1] = 0;
//        dst[offset + 2] = 0;
//        dst[offset + 3] = 1;
//        offset += nal_len;
//    }

//    long long tmStamp = m_param.b_vfr_input ? pic_out.i_dts :
//                        pic_out.i_dts*1000000*m_param.i_fps_den/m_param.i_fps_num;

    long long tmStamp = (long long)(m_frameId*1000LL/m_frameRate);
    opkt.frame.timestamp = tmStamp;
    opkt.frame.size = frameLen;
    opkt.frame.data = opkt.GetDataPtr();
    opkt.frame.media_type = MediaType_Video;

//    long long tmStamp = pkt.frame.timestamp / 1000;

    MediaChainImpl::onMsgProcessMedia(opkt);

//    LOGD("X264VideoEncoder::OnOpenEncoder: size=%d  i64=%ld\n", frameLen, tmStamp);
    return  0;
}

std::string X264VideoEncoder::GenAvc1() {
    x264_nal_t* encoded;
    int nal;

    if (x264_encoder_headers(m_handle, &encoded, &nal) < 0)
        return std::string();
    if (nal < 2)
        return std::string();

    unsigned char *sps = encoded[0].p_payload + 4;
    unsigned char *pps = encoded[1].p_payload + 4;
    short sps_size = encoded[0].i_payload-4;
    short pps_size = encoded[1].i_payload - 4;

    Bitstream bs ;

    bs << (unsigned char)1; /* version */
    bs << (unsigned char)sps[1]; /* profile */
    bs << (unsigned char)sps[2]; /* profile compat */
    bs << (unsigned char)sps[3]; /* level */
    bs << (unsigned char)0xff; /* 6 bits reserved (111111) + 2 bits nal size length - 1 (11) */
    bs << (unsigned char)0xe1; /* 3 bits reserved (111) + 5 bits number of sps (00001) */

    bs << sps_size;
    bs.write( sps, sps_size);
    bs << (unsigned char)1; /* number of pps */
    bs << pps_size ;
    bs.write(pps, pps_size);

    bs.getWriteBytes();

    std::string str = bs.getBuffer();
    return str;
}

std::string X264VideoEncoder::GetExtraConfig(MediaChain *chain) {
    return m_extraConfig;
}

std::string X264VideoEncoder::GenH264Config() {

    int nal;
    x264_nal_t* encoded;

    if (x264_encoder_headers(m_handle, &encoded, &nal) < 0)
        return std::string();
    if (nal < 2)
        return std::string();

    char head[encoded[0].i_payload + encoded[1].i_payload];
    memcpy(head, encoded[0].p_payload, encoded[0].i_payload);
    memcpy(head + encoded[0].i_payload, encoded[1].p_payload, encoded[1].i_payload);

    return std::string(head, encoded[0].i_payload + encoded[1].i_payload);
}


