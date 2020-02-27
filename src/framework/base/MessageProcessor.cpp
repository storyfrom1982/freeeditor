//
// Created by yongge on 20-2-20.
//

#include "MessageProcessor.h"


using namespace freee;

MessageProcessor::MessageProcessor() : mThreadId(0), isRunning(false), isStopped(false) {}

void MessageProcessor::StartProcessor(std::string name) {
    LOGD("MessageProcessor::StartProcessor [%s] enter\n", name.c_str());
    isRunning = true;
    isStopped = false;
    this->name = name;
    length = 256;
    put_index = get_index = 0;
    mPktList = std::vector<SmartPkt>(length);
    if (pthread_create(&mThreadId, nullptr, MessageProcessorThread, this) != 0) {
        LOGF("pthread_create failed\n");
    }
    LOGD("MessageProcessor::StartProcessor [%s] exit\n", name.c_str());
}

void MessageProcessor::StopProcessor() {
    LOGD("MessageProcessor::StopProcessor [%s] enter\n", name.c_str());
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
    mPktList.clear();
    LOGD("MessageProcessor::StopProcessor [%s] exit\n", name.c_str());
}

void MessageProcessor::ProcessMessage(SmartPkt pkt) {

    mLock.lock();

    while (0 == (length - put_index + get_index)){
        if (__is_true(isRunning)){
            mLock.wait();
        }else {
            mLock.unlock();
            return ;
        }
    }

    mPktList[put_index & (length - 1)] = pkt;
    put_index ++;

    mLock.signal();
    mLock.unlock();
}

void MessageProcessor::MessageProcessorLoop() {

    LOGD("MessageProcessor::MessageProcessorLoop [%s] enter\n", name.c_str());

    while (true) {

        mLock.lock();

        while (0 == (put_index - get_index)){
            if (__is_true(isRunning)){
                mLock.wait();
            }else {
                mLock.unlock();
                __set_true(isStopped);
                LOGD("MessageProcessor::MessageProcessorLoop [%s] exit\n", name.c_str());
                return ;
            }
        }

        SmartPkt pkt = mPktList[get_index & (length - 1)];
        mPktList[get_index & (length - 1)] = SmartPkt(0);
        get_index ++;

        mLock.signal();
        mLock.unlock();

        MessageProcess(pkt);
    }
}

void *MessageProcessor::MessageProcessorThread(void *p) {
    ((MessageProcessor *) p)->MessageProcessorLoop();
    return nullptr;
}
