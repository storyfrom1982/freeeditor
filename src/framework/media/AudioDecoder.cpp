//
// Created by yongge on 20-3-18.
//

#include "AudioDecoder.h"
#include "ffmpeg/FFmpegAudioDecoder.h"


using namespace freee;

AudioDecoder::AudioDecoder(const std::string name) : MessageChain(name)
{
    m_type = MediaType_Audio;
    StartProcessor();
}

AudioDecoder::~AudioDecoder()
{
    StopProcessor();
}

void AudioDecoder::onMsgOpen(Message msg)
{
    MessageChain *chain = static_cast<MessageChain *>(msg.GetObjectPtr());
    m_config = chain->GetConfig(this);
    LOGD("VideoDecoder::onMsgOpen config: %s\n", m_config.dump().c_str());
    OpenDecoder();
    MessageChain::onMsgOpen(msg);
    MessageChain::onMsgProcessEvent(NewMessage(MsgKey_ProcessEvent, MsgKey_Open));
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
