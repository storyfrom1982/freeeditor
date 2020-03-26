//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAPROCESSOR_H
#define ANDROID_MEDIAPROCESSOR_H


#include "AutoLock.h"
#include "MessageContext.h"

#include <vector>

namespace freee {


    class MessageProcessor : public MessageContext {

    public:
        MessageProcessor(std::string name) : MessageContext(name){}
        virtual ~MessageProcessor(){}

    protected:
        void StartProcessor(unsigned int messageQueueLength = 256){
            AutoLock lock(m_lock);
            if (!m_threadId){
                m_length = messageQueueLength;
                m_putIndex = m_getIndex = 0;
                m_messageQueue = std::vector<Message>(m_length);
                if (pthread_create(&m_threadId, nullptr, MessageProcessorThread, this) != 0) {
                    LOGF("pthread_create failed\n");
                }
            }
        }
        void StopProcessor(){
            ProcessMessage(NewMessage(MsgKey_Exit));
            m_lock.lock();
            pthread_t tid = m_threadId;
            m_threadId = 0;
            m_lock.unlock();
            if (tid){
                pthread_join(tid, nullptr);
                for (int i = 0; i < m_length; ++i){
                    m_messageQueue[i] = Message();
                }
            }
        }

        void ProcessMessage(Message msg){
            m_lock.lock();
            if (m_threadId == 0){
                m_lock.unlock();
                return;
            }
            while (0 == (m_length - m_putIndex + m_getIndex)){
                m_lock.wait();
            }
            m_messageQueue[m_putIndex & (m_length - 1)] = msg;
            m_putIndex ++;
            m_lock.signal();
            m_lock.unlock();
        }
        virtual void MessageProcess(Message msg){}

    private:
        void MessageProcessorLoop(){
            LOGD("MessageProcessor[%p] %40s thread start\n", pthread_self(), GetName().c_str());
            while (true) {
                m_lock.lock();
                while (0 == (m_putIndex - m_getIndex)){
                    m_lock.wait();
                }
                Message msg = m_messageQueue[m_getIndex & (m_length - 1)];
                m_messageQueue[m_getIndex & (m_length - 1)] = Message();
                m_getIndex ++;
                m_lock.signal();
                m_lock.unlock();
                if (msg.key() == MsgKey_Exit){
                    break;
                }
                MessageProcess(msg);
            }
            LOGD("MessageProcessor[%p] %40s thread stop\n", pthread_self(), GetName().c_str());
        }
        static void* MessageProcessorThread(void *p){
            ((MessageProcessor *) p)->MessageProcessorLoop();
            return nullptr;
        }

    private:
        unsigned int m_length;
        unsigned int m_putIndex;
        unsigned int m_getIndex;

        Lock m_lock;
        pthread_t m_threadId = 0;
        std::vector<Message> m_messageQueue;
    };

}



#endif //ANDROID_MEDIAPROCESSOR_H
