//
// Created by yongge on 19-5-20.
//

#include <vector>
#include <MsgKey.h>
#include <StreamCapture.h>
#include "EnvContext.h"
#include "MConfig.h"


using namespace freee;

static EnvContext *g_ctx = nullptr;

EnvContext* EnvContext::Instance() {
    if (g_ctx == nullptr){
        g_ctx = new EnvContext();
    }
    return g_ctx;
}

EnvContext::EnvContext() {
//    StreamCapture::initialize();
    StreamProcessor *listener = StreamCapture::getListener();
    addMessageListener(MsgKey_EnvCtx_CreateCapture, listener);
    addMessageListener(MsgKey_EnvCtx_RemoveCapture, listener);
};

EnvContext::~EnvContext() {
    StreamCapture::release();
    AutoLock lock(m_processorLock);
    m_processorMap.clear();
    g_ctx = nullptr;
}

sr_msg_t EnvContext::requestFromInputStream(sr_msg_t msg) {
    AutoLock lock(m_processorLock);
    if (!m_processorMap[msg.key].empty()){
        return m_processorMap[msg.key][0]->sendRequestToOutputStream(msg);
    }
    return __sr_null_msg;
}

void EnvContext::messageFromInputStream(sr_msg_t msg) {
    AutoLock lock(m_processorLock);
    if (!m_processorMap[msg.key].empty()){
        for (int i = 0; i < m_processorMap[msg.key].size(); ++i){
            m_processorMap[msg.key][i]->sendMessageToOutputStream(msg);
        }
    }else {
        switch (msg.key){
            case MsgKey_EnvCtx_StoragePath:
                if (__sr_msg_is_string(msg)){
                    sr_log_file_open(static_cast<const char *>(msg.p64));
                    __sr_msg_clear(msg);
                }
                break;
            default:
                break;
        }
    }
}

void EnvContext::addMessageListener(int32_t msgKey, StreamProcessor *processor) {
    AutoLock lock(m_processorLock);
    m_processorMap[msgKey].push_back(processor);
}

void EnvContext::removeMessageListener(int32_t msgKey, StreamProcessor *processor) {
    AutoLock lock(m_processorLock);
    if (!m_processorMap[msgKey].empty()){
        for (int i = 0; i < m_processorMap[msgKey].size(); ++i){
            if (m_processorMap[msgKey][i] == processor){
                m_processorMap[msgKey].erase(m_processorMap[msgKey].begin() + i);
                break;
            }
        }
    }
}


