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
        m_pktList = std::vector<SmartPkt>(m_length);
        if (pthread_create(&m_threadId, nullptr, MessageProcessorThread, this) != 0) {
            LOGF("pthread_create failed\n");
        }
    }
}

void MessageProcessor::StopProcessor() {
    ProcessMessage(SmartPkt(PktMsgExit));
    m_lock.lock();
    pthread_t tid = m_threadId;
    m_threadId = 0;
    m_lock.unlock();
    if (tid){
        pthread_join(tid, nullptr);
        m_threadId = 0;
    }
    m_pktList.clear();
}

void MessageProcessor::ProcessMessage(SmartPkt pkt) {
    m_lock.lock();
    while (0 == (m_length - m_putIndex + m_getIndex)){
        m_lock.wait();
    }
    m_pktList[m_putIndex & (m_length - 1)] = pkt;
    m_putIndex ++;
    m_lock.signal();
    m_lock.unlock();
}

void MessageProcessor::MessageProcessorLoop() {

    LOGD("MessageProcessor >>>>>>>>>>>>>>>>>>>>>>>>>> [%s] started\n", m_name.c_str());

    while (true) {

        m_lock.lock();

        while (0 == (m_putIndex - m_getIndex)){
            m_lock.wait();
        }

        SmartPkt pkt = m_pktList[m_getIndex & (m_length - 1)];
        m_pktList[m_getIndex & (m_length - 1)] = SmartPkt(0);
        m_getIndex ++;

        m_lock.signal();
        m_lock.unlock();

        if (pkt.GetKey() == PktMsgExit){
            break;
        }

        MessageProcess(pkt);
    }

    LOGD("MessageProcessor >>>>>>>>>>>>>>>>>>>>>>>>>> [%s] stopped\n", m_name.c_str());
}

void *MessageProcessor::MessageProcessorThread(void *p) {
    ((MessageProcessor *) p)->MessageProcessorLoop();
    return nullptr;
}
