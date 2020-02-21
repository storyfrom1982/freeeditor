//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAPROCESSOR_H
#define ANDROID_MEDIAPROCESSOR_H


#include <MediaBufferPool.h>
#include "AutoLock.h"
#include <string>
#include <vector>

namespace freee {


    class MediaProcessor {

    public:
        virtual ~MediaProcessor(){}

    protected:
        void StartProcessor(std::string name);
        void StopProcessor();

        virtual void ProcessPacket(MediaPacket pkt);
        virtual void PacketProcess(MediaPacket pkt){}

    private:
        void MediaProcessorLoop();
        static void* MediaProcessorThread(void *p);

    private:
        bool isRunning;
        bool isStopped;

        std::string name;

        Lock mLock;
        pthread_t mThreadId;

        std::vector<MediaPacket> mPktList;
    };

}



#endif //ANDROID_MEDIAPROCESSOR_H
