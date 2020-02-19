//
// Created by yongge on 20-2-17.
//

#ifndef ANDROID_SRMSG_H
#define ANDROID_SRMSG_H


#include <string>
#include <vector>
#include <AutoLock.h>

#include "SrBufferPool.h"


namespace freee {

    class SrMessageQueue {

    public:
        virtual ~SrMessageQueue(){}

    protected:
        void StartProcessor(std::string name);
        void StopProcessor();
        void PutMessage(SrPkt msg);
        virtual void MessageProcessor(SrPkt msg){}

    private:
        void MessageProcessorLoop();
        static void* MessageProcessorThread(void *p);

    private:
        bool running;
        bool stopped;

        std::string name;

        Lock mLock;
        pthread_t mTid;
        std::vector<SrPkt> mMessageList;
    };

}



#endif //ANDROID_SRMSG_H
