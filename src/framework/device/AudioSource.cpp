//
// Created by yongge on 20-2-15.
//

#include <MConfig.h>
#include <android/MediaContext.h>
#include "AudioSource.h"


using namespace freee;


enum {
    PutMsg_Open = 1,
    PutMsg_Close = 2,
    PutMsg_Start = 3,
    PutMsg_Stop = 4,
};

enum {
    OnPutMsg_Opened = 1,
    OnPutMsg_Closed = 2,
    OnPutMsg_Started = 3,
    OnPutMsg_Stopped = 4,
    OnPutMsg_ProcessSound = 5
};


AudioSource::AudioSource(int mediaType, int mediaNumber, std::string mediaName)
        : MediaChainImpl(mediaType, mediaNumber, mediaName) {
    MessageContext *context = MediaContext::Instance().ConnectMicrophone();
    ConnectContext(context);
}

AudioSource::~AudioSource() {
    LOGD("AudioSource::~AudioSource\n");
    Close(this);
    DisconnectContext();
    MediaContext::Instance().DisconnectMicrophone();
}

void AudioSource::onRecvMessage(SmartPkt msg) {
//    LOGD("AudioSource::onRecvMessage data %d\n", msg.GetKey());
}

SmartPkt AudioSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void AudioSource::Open(MediaChain *chain) {
    mConfig = chain->GetConfig(this);
    std::string str = mConfig.dump();
    SmartPkt msg(PutMsg_Open, str.c_str(), str.length());
    SendMessage(msg);
}

void AudioSource::Close(MediaChain *chain) {
    SmartPkt pkt(PutMsg_Close);
    SendMessage(pkt);
}

void AudioSource::Start(MediaChain *chain) {
    SmartPkt pkt(PutMsg_Start);
    SendMessage(pkt);
}

void AudioSource::Stop(MediaChain *chain) {
    SmartPkt pkt(PutMsg_Stop);
    SendMessage(pkt);
}

void AudioSource::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    MediaChainImpl::ProcessMedia(chain, pkt);
}

