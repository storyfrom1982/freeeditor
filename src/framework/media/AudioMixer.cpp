//
// Created by yongge on 20-5-13.
//

#include "AudioMixer.h"


using namespace freee;


typedef struct InputChannel {
    int delay;
    int sampleRate;
    int channelCount;
    int bytesPerSample;
    sr_pipe_t *pipe;
}InputChannel;


freee::AudioMixer::AudioMixer(const std::string &name) : MediaPlugin(name)
{}

freee::AudioMixer::~AudioMixer()
{

}

int freee::AudioMixer::ProcessMedia(freee::MediaPlugin *plugin, freee::Message msg)
{
    return MediaPlugin::ProcessMedia(plugin, msg);
}

void AudioMixer::onMsgOpen(Message msg)
{
    MessageChain *chain = (MessageChain*) msg.obj();
    m_config = chain->GetConfig(this);
    InputChannel *channel = (InputChannel*) calloc(1, sizeof(InputChannel));
    m_channels[chain] = channel;
    MessageChain::onMsgOpen(msg);
}

void AudioMixer::onMsgClose(Message msg)
{
    MessageChain::onMsgClose(msg);
}

void AudioMixer::onMsgProcessData(Message msg)
{
    MessageChain::onMsgProcessData(msg);
}
