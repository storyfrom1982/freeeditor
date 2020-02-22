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
        void MediaProcessorLoop();
        static void* MediaProcessorThread(void *p);

    private:
        bool isRunning;
        bool isStopped;

        std::string name;

        Lock mLock;
        pthread_t mThreadId;

        std::vector<SmartPkt> mPktList;
    };

}



#endif //ANDROID_MEDIAPROCESSOR_H
