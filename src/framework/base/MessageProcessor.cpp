//
// Created by yongge on 20-2-20.
//

#include <BufferPool.h>
#include "MessageProcessor.h"


using namespace freee;

MessageProcessor::MessageProcessor() : m_threadId(0) {}

void MessageProcessor::StartProcessor(std::string name) {
    AutoLock lock(m_lock);
    if (!m_threadId){
        this->m_name = name;
        m_length = 256;
        m_putIndex = m_getIndex = 0;
        m_pktQueue = std::vector<Message>(m_length);
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
        m_threadId = 0;
    }
    m_pktQueue.clear();
}

void MessageProcessor::ProcessMessage(Message pkt) {
    m_lock.lock();
    while (0 == (m_length - m_putIndex + m_getIndex)){
        m_lock.wait();
    }
    m_pktQueue[m_putIndex & (m_length - 1)] = pkt;
    m_putIndex ++;
    m_lock.signal();
    m_lock.unlock();
}

void MessageProcessor::MessageProcessorLoop() {

    LOGD("[THREAD]<START>[%s] [%p]\n", m_name.c_str(), pthread_self());

    while (true) {

        m_lock.lock();

        while (0 == (m_putIndex - m_getIndex)){
            m_lock.wait();
        }

        Message pkt = m_pktQueue[m_getIndex & (m_length - 1)];
        m_pktQueue[m_getIndex & (m_length - 1)] = Message(0);
        m_getIndex ++;

        m_lock.signal();
        m_lock.unlock();

        if (pkt.GetKey() == MsgKey_Exit){
            break;
        }

        MessageProcess(pkt);
    }

    LOGD("[THREAD]<STOPPED>[%s] [%p]\n", m_name.c_str(), pthread_self());
}

void *MessageProcessor::MessageProcessorThread(void *p) {
    ((MessageProcessor *) p)->MessageProcessorLoop();
    return nullptr;
}
