//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAPROCESSOR_H
#define ANDROID_MEDIAPROCESSOR_H


#include <MessagePool.h>
#include "AutoLock.h"
#include "MessageContext.h"
#include <string>
#include <vector>

namespace freee {


    class MessageProcessor : public MessageContext {

    public:
        MessageProcessor(std::string name);
        virtual ~MessageProcessor(){}

    protected:
        void StartProcessor();
        void StopProcessor();

        virtual void ProcessMessage(Message msg);
        virtual void MessageProcess(Message msg){}

    private:
        void MessageProcessorLoop();
        static void* MessageProcessorThread(void *p);

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
