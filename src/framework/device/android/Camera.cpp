//
// Created by yongge on 19-6-16.
//

#include "Camera.h"

freee::Camera::Camera(freee::IMsgListener *listener)
        : VideoSource(listener) {

}

freee::Camera::~Camera() {

}

void freee::Camera::openSource(json cfg) {
    std::string s = cfg.dump();
    LOGD("AndroidCamera::openSource: %s", s.c_str());
    sr_msg_t msg = __sr_msg_malloc(MSG_KEY_SendCmd_OpenSource, s.length());
    memcpy(msg.ptr, s.c_str(), s.length());
    sendMessageToUpstream(msg);
    LOGD("AndroidCamera::openSource: exit", s.c_str());
}

void freee::Camera::closeSource() {
    sr_msg_t msg = {0};
    msg.key = MSG_KEY_SendCmd_CloseSource;
    sendMessageToUpstream(msg);
}

void freee::Camera::startCapture() {
    LOGD("AndroidCamera::startCapture enter");
    sr_msg_t msg = {0};
    msg.key = MSG_KEY_SendCmd_StartCapture;
    sendMessageToUpstream(msg);
    LOGD("AndroidCamera::startCapture exit");
}

void freee::Camera::stopCapture() {
    sr_msg_t msg = {0};
    msg.key = MSG_KEY_SendCmd_StopCapture;
    sendMessageToUpstream(msg);
}

sr_msg_t freee::Camera::onRequestFromUpstream(sr_msg_t msg) {
    switch (msg.key){
        case MSG_KEY_RecvCmd_UpdateVideoConfig:
            m_videoConfig = json::parse(std::string((const char *)msg.ptr));
            LOGD("update config =======: %s\n", m_videoConfig.dump().c_str());
            break;
        case MSG_KEY_RecvCmd_PreviewFrame:
            LOGD("video frame =======: %p\n", msg.ptr);
            break;
        default:
            break;
    }
    return __sr_bad_msg;
}

sr_msg_t freee::Camera::onRequestFromDownstream(sr_msg_t msg) {
    return __sr_bad_msg;
}
