//
// Created by yongge on 20-2-20.
//

#include <BufferPool.h>
#include "MessageProcessor.h"


using namespace freee;

MessageProcessor::MessageProcessor() : mThreadId(0) {}

void MessageProcessor::StartProcessor(std::string name) {
    AutoLock lock(mLock);
    if (!mThreadId){
        this->name = name;
        length = 256;
        put_index = get_index = 0;
        mPktList = std::vector<SmartPkt>(length);
        if (pthread_create(&mThreadId, nullptr, MessageProcessorThread, this) != 0) {
            LOGF("pthread_create failed\n");
        }
    }
}

void MessageProcessor::StopProcessor() {
    ProcessMessage(SmartPkt(PktMsgExit));
    mLock.lock();
    pthread_t tid = mThreadId;
    mThreadId = 0;
    mLock.unlock();
    if (tid){
        pthread_join(tid, nullptr);
        mThreadId = 0;
    }
    mPktList.clear();
}

void MessageProcessor::ProcessMessage(SmartPkt pkt) {
    mLock.lock();
    while (0 == (length - put_index + get_index)){
        mLock.wait();
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
            mLock.wait();
        }

        SmartPkt pkt = mPktList[get_index & (length - 1)];
        mPktList[get_index & (length - 1)] = SmartPkt(0);
        get_index ++;

        mLock.signal();
        mLock.unlock();

        if (pkt.msg.key == 0){
            break;
        }

        MessageProcess(pkt);
    }

    LOGD("MessageProcessor::MessageProcessorLoop [%s] exit\n", name.c_str());
}

void *MessageProcessor::MessageProcessorThread(void *p) {
    ((MessageProcessor *) p)->MessageProcessorLoop();
    return nullptr;
}
