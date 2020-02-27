//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAPROCESSOR_H
#define ANDROID_MEDIAPROCESSOR_H


#include <BufferPool.h>
#include "AutoLock.h"
#include <string>
#include <vector>

namespace freee {


    class MessageProcessor {

    public:
        MessageProcessor();
        virtual ~MessageProcessor(){}

    protected:
        void StartProcessor(std::string name);
        void StopProcessor();

        virtual void ProcessMessage(SmartPkt pkt);
        virtual void MessageProcess(SmartPkt pkt){}

    private:
        void MessageProcessorLoop();
        static void* MessageProcessorThread(void *p);

    private:
        bool isRunning;
        bool isStopped;

        unsigned int length;
        unsigned int put_index;
        unsigned int get_index;

        std::string name;

        Lock mLock;
        pthread_t mThreadId;

        std::vector<SmartPkt> mPktList;
    };

}



#endif //ANDROID_MEDIAPROCESSOR_H
