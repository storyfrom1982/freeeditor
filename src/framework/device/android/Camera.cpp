//
// Created by yongge on 19-6-16.
//

#include "Camera.h"

#include <MsgKey.h>


using namespace freee;


Camera::Camera(DeviceContext *ctx)
        : VideoSource(ctx) {

}

Camera::~Camera() {

}

void freee::Camera::openSource(json cfg) {
    std::string s = cfg.dump();
    LOGD("AndroidCamera::openSource: %s", s.c_str());
    putMessage(VideoSource_Open, cfg.dump());
    LOGD("AndroidCamera::openSource: exit");
}

void freee::Camera::closeSource() {
    putMessage(VideoSource_Close, "");
}

void freee::Camera::startCapture() {
    LOGD("AndroidCamera::startCapture enter");
    putMessage(VideoSource_Start, "");
    LOGD("AndroidCamera::startCapture exit");
}

void freee::Camera::stopCapture() {
    putMessage(VideoSource_Stop, "");
}

int Camera::onPutObject(int type, void *obj) {
    return 0;
}

void *Camera::onGetObject(int type) {
    return nullptr;
}

int Camera::onPutMessage(int cmd, std::string msg) {
    return 0;
}

std::string Camera::onGetMessage(int cmd) {
    return std::string();
}

int Camera::onPutData(void *data, int size) {
//    LOGD("camera data size: %d", size);
    processData(data, size);
    return 0;
}

void *Camera::onGetBuffer() {
    return nullptr;
}

int
Camera::imageFilter(void *src, int src_w, int src_h, int src_fmt, void *dst, int dst_w, int dst_h,
                    int dst_fmt, int rotate) {
    return VideoSource::imageFilter(src, src_w, src_h, src_fmt, dst, dst_w, dst_h, dst_fmt, rotate);
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
