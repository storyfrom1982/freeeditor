//
// Created by yongge on 20-3-18.
//

#include "AudioDecoder.h"
#include "FFmpegAudioDecoder.h"


using namespace freee;

AudioDecoder::AudioDecoder(const std::string &name, int type) : MessageChain(name, type)
{
    StartProcessor();
}

AudioDecoder::~AudioDecoder()
{
    StopProcessor();
}

void AudioDecoder::onMsgOpen(Message msg)
{
    MessageChain *chain = static_cast<MessageChain *>(msg.GetPtr());
    m_config = chain->GetConfig(this);
    LOGD("VideoDecoder::onMsgOpen config: %s\n", m_config.dump().c_str());
    OpenDecoder();
    MessageChain::onMsgOpen(msg);
    Message event(MsgKey_ProcessEvent);
    event.SetSubKey(MsgKey_Open);
    MessageChain::onMsgProcessEvent(event);
}

void AudioDecoder::onMsgClose(Message msg)
{
    CloseDecoder();
}

void AudioDecoder::onMsgProcessData(Message msg)
{
    DecodeAudio(msg);
}

void AudioDecoder::onMsgControl(Message msg)
{
    MessageChain::onMsgControl(msg);
}

AudioDecoder *AudioDecoder::Create(std::string name)
{
    return new FFmpegAudioDecoder();
}
