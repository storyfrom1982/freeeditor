//
// Created by yongge on 20-2-20.
//

#include "MessageProcessor.h"


using namespace freee;

MessageProcessor::MessageProcessor() : mThreadId(0), isRunning(false), isStopped(false) {}

void MessageProcessor::StartProcessor(std::string name) {
    LOGD("MediaProcessor::StartProcessor [%s] enter\n", name.c_str());
    isRunning = true;
    isStopped = false;
    this->name = name;
    if (pthread_create(&mThreadId, nullptr, MediaProcessorThread, this) != 0) {
        LOGF("pthread_create failed\n");
    }
    LOGD("MediaProcessor::StartProcessor [%s] exit\n", name.c_str());
}

void MessageProcessor::StopProcessor() {
    LOGD("MediaProcessor::StopProcessor [%s] enter\n", name.c_str());
    __set_false(isRunning);
    mLock.lock();
    pthread_t tid = mThreadId;
    mThreadId = 0;
    mLock.unlock();
    if (tid != 0){
        while(__is_false(isStopped)){
            mLock.lock();
            mLock.broadcast();
            mLock.unlock();
            nanosleep((const struct timespec[]){{0, 100000L}}, nullptr);
        }
        pthread_join(tid, nullptr);
    }
    LOGD("MediaProcessor::StopProcessor [%s] exit\n", name.c_str());
}

void MessageProcessor::ProcessMessage(SmartPkt pkt) {
    if (__is_true(isRunning)){
        mLock.lock();
        mPktList.push_back(pkt);
        mLock.signal();
        mLock.unlock();
    }
}

void MessageProcessor::MediaProcessorLoop() {

    LOGD("MediaProcessor::MediaProcessorLoop [%s] enter\n", name.c_str());

    while (true) {

        mLock.lock();

        while (mPktList.empty()){
            if (__is_true(isRunning)){
                mLock.wait();
            }else {
                __set_true(isStopped);
                LOGD("MediaProcessor::MediaProcessorLoop [%s] exit\n", name.c_str());
                mLock.unlock();
                return;
            }
        }

        auto it = mPktList.begin();
        SmartPkt pkt = *it;
        mPktList.erase(it);

        mLock.signal();
        mLock.unlock();

        MessageProcess(pkt);
    }
}

void *MessageProcessor::MediaProcessorThread(void *p) {
    ((MessageProcessor *) p)->MediaProcessorLoop();
    return nullptr;
}
