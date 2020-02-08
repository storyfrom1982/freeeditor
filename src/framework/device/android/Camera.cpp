//
// Created by yongge on 19-6-16.
//

#include "Camera.h"

#include <MsgKey.h>


using namespace freee;


Camera::Camera(MessageContext *ctx){
    SetContextHandler(ctx);
}

Camera::~Camera() {
    LOGD("Camera::~Camera enter");
    sr_message_t msg = __sr_null_msg;
    msg.key = -1;
    PutMessage(msg);
    LOGD("Camera::~Camera exit");
}

void freee::Camera::openSource(json cfg) {
    sr_message_t msg;
    std::string str = cfg.dump();
    LOGD("AndroidCamera::openSource: %s", str.c_str());
    msg.key = VideoSource_Open;
    msg.size = str.length();
    msg.ptr = strdup(str.c_str());
    PutMessage(msg);
    LOGD("AndroidCamera::openSource: exit");
}

void freee::Camera::closeSource() {
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Close;
    PutMessage(msg);
}

void freee::Camera::startCapture() {
    LOGD("AndroidCamera::startCapture enter");
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Start;
    PutMessage(msg);
    LOGD("AndroidCamera::startCapture exit");
}

void freee::Camera::stopCapture() {
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Stop;
    PutMessage(msg);
}

void Camera::OnPutMessage(sr_message_t msg) {
    processData(msg);
}

sr_message_t Camera::OnGetMessage(sr_message_t msg) {
    return sr_message_t();
}

//sr_msg_t freee::Camera::requestFromInputStream(sr_msg_t msg) {
//    switch (msg.key){
//        case MsgKey_Video_Source_FinalConfig:
//            m_videoConfig = json::parse(std::string((const char *)msg.p64));
//            __sr_msg_clear(msg);
//            LOGD("update config =======: %s\n", m_videoConfig.dump().c_str());
//            break;
//        case MsgKey_Video_Source_ProvideFrame:
////            LOGD("video frame =======: %p\n", msg.ptr);
//            break;
//        default:
//            break;
//    }
//    return __sr_null_msg;
//}
//
//sr_msg_t freee::Camera::requestFromOutputStream(sr_msg_t msg) {
//    return __sr_null_msg;
//}
