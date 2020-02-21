//
// Created by yongge on 20-2-17.
//

#ifndef ANDROID_SRMSG_H
#define ANDROID_SRMSG_H


#include <string>
#include <vector>
#include <AutoLock.h>

#include "MediaBufferPool.h"


namespace freee {

    class SrMessageQueue {

    public:
        virtual ~SrMessageQueue(){}

    protected:
        void StartProcessor(std::string name);
        void StopProcessor();
        void PutMessage(MediaPacket msg);
        virtual void MessageProcessor(MediaPacket msg){}

    private:
        void MessageProcessorLoop();
        static void* MessageProcessorThread(void *p);

    private:
        bool running;
        bool stopped;

        std::string name;

        Lock mLock;
        pthread_t mTid;
        std::vector<MediaPacket> mMessageList;
    };

}



#endif //ANDROID_SRMSG_H
