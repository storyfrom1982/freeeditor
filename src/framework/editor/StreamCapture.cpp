//
// Created by yongge on 19-8-14.
//

#include "StreamCapture.h"
#include <EnvContext.h>


using namespace freee;


class CaptureListener : public StreamProcessor {

protected:

    sr_msg_t requestFromInputStream(sr_msg_t msg) override {
        StreamCapture *capture = nullptr;
        if (msg.key == MsgKey_EnvCtx_CreateCapture) {
            AutoLock lock(m_captureListLock);
            capture = StreamCapture::create(msg);
            m_captureList.push_back(capture);
            msg = __sr_ok_msg;
            msg.type = SR_MSG_TYPE_INTEGER;
            msg.ptr = capture;
            return msg;
        } else if (msg.key == MsgKey_EnvCtx_RemoveCapture) {
            if (msg.ptr != nullptr) {
                capture = static_cast<StreamCapture *>(msg.ptr);
                AutoLock lock(m_captureListLock);
                if (!m_captureList.empty()) {
                    for (int i = 0; i < m_captureList.size(); ++i) {
                        if (m_captureList[i] == capture) {
                            m_captureList.erase(m_captureList.begin() + i);
                            delete capture;
                            return __sr_ok_msg;
                        }
                    }
                }
            }
        }
        return __sr_null_msg;
    }

private:

    Mutex m_captureListLock;
    std::vector<StreamCapture *> m_captureList;
};


static CaptureListener *s_listener = nullptr;


void StreamCapture::initialize() {
    s_listener = new CaptureListener();
    EnvContext::Instance()->addMessageListener(MsgKey_EnvCtx_CreateCapture, s_listener);
    EnvContext::Instance()->addMessageListener(MsgKey_EnvCtx_RemoveCapture, s_listener);
}

void StreamCapture::release() {
    if (s_listener != nullptr){
        EnvContext::Instance()->removeMessageListener(MsgKey_EnvCtx_CreateCapture, s_listener);
        EnvContext::Instance()->removeMessageListener(MsgKey_EnvCtx_RemoveCapture, s_listener);
        delete s_listener;
        s_listener = nullptr;
    }
}

StreamCapture *StreamCapture::create(sr_msg_t msg) {
    return new Editor(msg);
}

void StreamCapture::messageFromInputStream(sr_msg_t msg) {

}

void StreamCapture::messageFromOutputStream(sr_msg_t msg) {

}

sr_msg_t StreamCapture::requestFromInputStream(sr_msg_t msg) {
    return __sr_null_msg;
}

sr_msg_t StreamCapture::requestFromOutputStream(sr_msg_t msg) {
    return __sr_null_msg;
}

StreamProcessor *StreamCapture::getListener() {
    return new CaptureListener();
}
