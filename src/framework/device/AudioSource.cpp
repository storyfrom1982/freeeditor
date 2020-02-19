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
    MessageContext *context = MediaContext::Instance()->CreateMicrophone();
    ConnectContext(context);
}

AudioSource::~AudioSource() {
    Close();
}

void AudioSource::onReceiveMessage(SrPkt msg) {
//    LOGD("AudioSource::onRecvFrom data size=%d\n", msg.type);
//    SrMessage buffer;
//    buffer.buffer->data = static_cast<unsigned char *>(msg.ptr);
//    mEncoder->EncodeAudioData(buffer);
}

SrPkt AudioSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void AudioSource::Open(json& cfg) {
    SrPkt pkt;
    std::string str = cfg.dump();
    pkt.msg.key = PutMsg_Open;
    pkt.msg.size = str.length();
    pkt.msg.js = strdup(str.c_str());
    SendMessage(pkt);
    mEncoder->OpenAudioEncoder(cfg);
}

void AudioSource::Close() {
    SrPkt pkt;
    pkt.msg.key = PutMsg_Close;
    SendMessage(pkt);
    mEncoder->CloseAudioEncoder();
}

void AudioSource::Start() {
    SrPkt pkt;
    pkt.msg.key = PutMsg_Start;
    SendMessage(pkt);
}

void AudioSource::Stop() {
    SrPkt pkt;
    pkt.msg.key = PutMsg_Stop;
    SendMessage(pkt);
}

void AudioSource::SetEncoder(AudioEncoder *encoder) {
    mEncoder = encoder;
}

