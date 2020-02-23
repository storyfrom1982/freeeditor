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


AudioSource::AudioSource() {
    MessageContext *context = MediaContext::Instance().ConnectMicrophone();
    ConnectContext(context);
}

AudioSource::~AudioSource() {
    Close();
}

void AudioSource::onRecvMessage(SmartMsg msg) {
//    LOGD("AudioSource::onRecvFrom data size=%d\n", msg.type);
//    SrMessage buffer;
//    buffer.buffer->data = static_cast<unsigned char *>(msg.ptr);
//    mEncoder->EncodeAudioData(buffer);
}

SmartMsg AudioSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void AudioSource::Open(json& cfg) {
    SmartMsg msg(PutMsg_Open, cfg.dump());
    SendMessage(msg);
    mEncoder->OpenAudioEncoder(cfg);
}

void AudioSource::Close() {
    SmartMsg msg(PutMsg_Close);
    SendMessage(msg);
    mEncoder->CloseAudioEncoder();
}

void AudioSource::Start() {
    SmartMsg msg(PutMsg_Start);
    SendMessage(msg);
}

void AudioSource::Stop() {
    SmartMsg msg(PutMsg_Stop);
    SendMessage(msg);
}

void AudioSource::SetEncoder(AudioEncoder *encoder) {
    mEncoder = encoder;
}

