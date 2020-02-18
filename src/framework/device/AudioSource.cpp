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

void AudioSource::OnPutMessage(sr_message_t msg) {
//    LOGD("AudioSource::OnPutMessage data size=%d\n", msg.type);
//    SrMessage buffer;
//    buffer.buffer->data = static_cast<unsigned char *>(msg.ptr);
//    mEncoder->EncodeAudioData(buffer);
}

sr_message_t AudioSource::OnGetMessage(sr_message_t msg) {
    return MessageContext::OnGetMessage(msg);
}

void AudioSource::Open(json& cfg) {
    sr_message_t msg = __sr_null_msg;
    std::string str = cfg.dump();
    msg.key = PutMsg_Open;
    msg.type = str.length();
    msg.str = strdup(str.c_str());
    PutMessage(msg);
    mEncoder->OpenAudioEncoder(cfg);
}

void AudioSource::Close() {
    sr_message_t msg = __sr_null_msg;
    msg.key = PutMsg_Close;
    PutMessage(msg);
    mEncoder->CloseAudioEncoder();
}

void AudioSource::Start() {
    sr_message_t msg = __sr_null_msg;
    msg.key = PutMsg_Start;
    PutMessage(msg);
}

void AudioSource::Stop() {
    sr_message_t msg = __sr_null_msg;
    msg.key = PutMsg_Stop;
    PutMessage(msg);
}

void AudioSource::SetEncoder(AudioEncoder *encoder) {
    mEncoder = encoder;
}

