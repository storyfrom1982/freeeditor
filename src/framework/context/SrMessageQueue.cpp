//
// Created by yongge on 20-2-17.
//

#include "SrMessageQueue.h"


using namespace freee;

void *SrMessageQueue::MessageProcessorThread(void *p) {
    ((SrMessageQueue*)p)->MessageProcessorLoop();
    return nullptr;
}

void SrMessageQueue::StartProcessor(std::string name) {
    LOGD("StartProcessor [%s] enter\n", name.c_str());
    running = true;
    stopped = false;
    this->name = name;
    if (pthread_create(&mTid, NULL, MessageProcessorThread, this) != 0) {
        LOGF("pthread_create failed\n");
    }
    LOGD("StartProcessor [%s] exit\n", name.c_str());
}

void SrMessageQueue::StopProcessor() {
    LOGD("StopProcessor [%s] enter\n", name.c_str());
    __set_false(running);
    pthread_t tid = mTid;
    mTid = 0;
    if (tid != 0){
        while(__is_false(stopped)){
            mLock.lock();
            mLock.broadcast();
            mLock.unlock();
            nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
        }
        pthread_join(tid, NULL);
    }
    LOGD("StopProcessor [%s] exit\n", name.c_str());
}

void SrMessageQueue::PutMessage(SmartPkt msg) {
    if (__is_true(running)){
        mLock.lock();
        mMessageList.push_back(msg);
        mLock.signal();
        mLock.unlock();
    }
}

void SrMessageQueue::MessageProcessorLoop() {

    LOGD("message processor [%s] enter\n", name.c_str());

    while (true) {

        mLock.lock();

        while (mMessageList.empty()){
            if (__is_true(running)){
                mLock.wait();
            }else {
                __set_true(stopped);
                LOGD("message processor [%s] exit\n", name.c_str());
                return;
            }
        }

        auto it = mMessageList.begin();
        SmartPkt msg = *it;
        mMessageList.erase(it);

        mLock.signal();
        mLock.unlock();

        MessageProcessor(msg);
    }
}