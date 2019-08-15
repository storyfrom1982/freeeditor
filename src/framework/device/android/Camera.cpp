//
// Created by yongge on 19-6-16.
//

#include "Camera.h"

#include <MsgKey.h>


freee::Camera::Camera(freee::StreamProcessor *listener)
        : VideoSource(listener) {

}

freee::Camera::~Camera() {

}

void freee::Camera::openSource(json cfg) {
    std::string s = cfg.dump();
    LOGD("AndroidCamera::openSource: %s", s.c_str());
    sr_msg_t msg = __sr_msg_malloc(MsgKey_Video_Source_Open, s.length());
    memcpy(msg.ptr, s.c_str(), s.length());
    sendMessageToInputStream(msg);
    LOGD("AndroidCamera::openSource: exit", s.c_str());
}

void freee::Camera::closeSource() {
    sr_msg_t msg = {0};
    msg.key = MsgKey_Video_Source_Close;
    sendMessageToInputStream(msg);
}

void freee::Camera::startCapture() {
    LOGD("AndroidCamera::startCapture enter");
    sr_msg_t msg = {0};
    msg.key = MsgKey_Video_Source_StartCapture;
    sendMessageToInputStream(msg);
    LOGD("AndroidCamera::startCapture exit");
}

void freee::Camera::stopCapture() {
    sr_msg_t msg = {0};
    msg.key = MsgKey_Video_Source_StopCapture;
    sendMessageToInputStream(msg);
}

sr_msg_t freee::Camera::requestFromInputStream(sr_msg_t msg) {
    switch (msg.key){
        case MsgKey_Video_Source_FinalConfig:
            m_videoConfig = json::parse(std::string((const char *)msg.ptr));
            LOGD("update config =======: %s\n", m_videoConfig.dump().c_str());
            break;
        case MsgKey_Video_Source_ProvideFrame:
//            LOGD("video frame =======: %p\n", msg.ptr);
            break;
        default:
            break;
    }
    return __sr_null_msg;
}

sr_msg_t freee::Camera::requestFromOutputStream(sr_msg_t msg) {
    return __sr_null_msg;
}
