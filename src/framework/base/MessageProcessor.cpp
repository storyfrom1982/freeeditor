//
// Created by yongge on 20-2-20.
//

#include <BufferPool.h>
#include "MessageProcessor.h"


using namespace freee;

MessageProcessor::MessageProcessor(std::string name) : MessageContext(name) {}

void MessageProcessor::StartProcessor() {
    AutoLock lock(m_lock);
    if (!m_threadId){
        m_length = 256;
        m_putIndex = m_getIndex = 0;
        m_messageQueue = std::vector<Message>(m_length);
        if (pthread_create(&m_threadId, nullptr, MessageProcessorThread, this) != 0) {
            LOGF("pthread_create failed\n");
        }
    }
}

void MessageProcessor::StopProcessor() {
    ProcessMessage(Message(MsgKey_Exit));
    m_lock.lock();
    pthread_t tid = m_threadId;
    m_threadId = 0;
    m_lock.unlock();
    if (tid){
        pthread_join(tid, nullptr);
    }
    m_messageQueue.clear();
}

void MessageProcessor::ProcessMessage(Message msg) {
    m_lock.lock();
    while (0 == (m_length - m_putIndex + m_getIndex)){
        m_lock.wait();
    }
    m_messageQueue[m_putIndex & (m_length - 1)] = msg;
    m_putIndex ++;
    m_lock.signal();
    m_lock.unlock();
}

void MessageProcessor::MessageProcessorLoop() {

    LOGD("[THREAD]<START>[%s] [%p]\n", GetName().c_str(), pthread_self());

    while (true) {

        m_lock.lock();

        while (0 == (m_putIndex - m_getIndex)){
            m_lock.wait();
        }

        Message pkt = m_messageQueue[m_getIndex & (m_length - 1)];
        m_messageQueue[m_getIndex & (m_length - 1)] = Message();
        m_getIndex ++;

        m_lock.signal();
        m_lock.unlock();

        if (pkt.GetKey() == MsgKey_Exit){
            break;
        }

        MessageProcess(pkt);
    }

    LOGD("[THREAD]<STOPPED>[%s] [%p]\n", GetName().c_str(), pthread_self());
}

void *MessageProcessor::MessageProcessorThread(void *p) {
    ((MessageProcessor *) p)->MessageProcessorLoop();
    return nullptr;
}
