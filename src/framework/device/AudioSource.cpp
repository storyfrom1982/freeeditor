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

void AudioSource::onRecvMessage(SmartMsg msg) {
//    LOGD("AudioSource::onRecvMessage data %d\n", msg.GetKey());
}

SmartMsg AudioSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void AudioSource::Open(MediaChain *chain) {
    mConfig = chain->GetConfig(this);
    SmartMsg msg(PutMsg_Open, mConfig.dump());
    SendMessage(msg);
}

void AudioSource::Close(MediaChain *chain) {
    SmartMsg msg(PutMsg_Close);
    SendMessage(msg);
}

void AudioSource::Start(MediaChain *chain) {
    SmartMsg msg(PutMsg_Start);
    SendMessage(msg);
}

void AudioSource::Stop(MediaChain *chain) {
    SmartMsg msg(PutMsg_Stop);
    SendMessage(msg);
}

void AudioSource::ProcessMedia(MediaChain *chain, SmartPkt pkt) {
    MediaChainImpl::ProcessMedia(chain, pkt);
}

