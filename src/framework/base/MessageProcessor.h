//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAPROCESSOR_H
#define ANDROID_MEDIAPROCESSOR_H


#include <BufferPool.h>
#include "AutoLock.h"
#include "MessageContext.h"
#include <string>
#include <vector>

namespace freee {


    class MessageProcessor : public MessageContext {

    public:
        MessageProcessor();
        virtual ~MessageProcessor(){}

    protected:
        void StartProcessor(std::string name);
        void StopProcessor();

        virtual void ProcessMessage(Message pkt);
        virtual void MessageProcess(Message pkt){}

    private:
        void MessageProcessorLoop();
        static void* MessageProcessorThread(void *p);

    private:
        std::string m_name;

        unsigned int m_length;
        unsigned int m_putIndex;
        unsigned int m_getIndex;

        Lock m_lock;
        pthread_t m_threadId;
        std::vector<Message> m_pktQueue;
    };

}



#endif //ANDROID_MEDIAPROCESSOR_H
