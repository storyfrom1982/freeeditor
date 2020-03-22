//
// Created by yongge on 20-3-22.
//

#include <android/MediaContext.h>
#include "AudioPlayer.h"


using namespace freee;

AudioPlayer::AudioPlayer(const std::string &name, int type) : MessageChain(name, type)
{
    MessageContext *context = MediaContext::Instance()->ConnectSpeaker();
    ConnectContext(context);
}

AudioPlayer::~AudioPlayer()
{
    DisconnectContext();
    MediaContext::Instance()->DisconnectSpeaker();
    sr_pipe_remove(&pipe);
}

void AudioPlayer::Open(MessageChain *chain)
{
    m_config = chain->GetConfig(this);
    pipe = sr_pipe_create(1 << 18);
    Message msg = NewJsonMessage(MsgKey_Open, m_config.dump());
    SendMessage(msg);
    SendMessage(Message(MsgKey_Start));
}

void AudioPlayer::Close(MessageChain *chain)
{
    sr_pipe_stop(pipe);
    SendMessage(Message(MsgKey_Stop));
    SendMessage(Message(MsgKey_Close));
}

void AudioPlayer::Start(MessageChain *chain)
{
    SendMessage(Message(MsgKey_Start));
}

void AudioPlayer::Stop(MessageChain *chain)
{
    SendMessage(Message(MsgKey_Stop));
}

void AudioPlayer::ProcessData(MessageChain *chain, Message msg)
{
    char tmp[2048];
    short *pcm = (short*)&tmp;
    for (int j=0; j<1024; j++){
        for (int c=0 ;c< 1; c++){
            float *f = (float *)msg.GetDataPtr();
            float t = f[j];
            if (t<-1.0f) t=-1.0f;
            else if (t>1.0f) t=1.0f;
            *pcm++ = t*32767.0f;
        }
    }
    sr_pipe_block_write(pipe, tmp, 2048);
}

void AudioPlayer::onRecvMessage(Message msg)
{
//    LOGD("AudioPlayer::onRecvMessage %lld\n", msg.frame.timestamp);
    if (sr_pipe_readable(pipe) < msg.GetMsgLength()){
        return;
    }
    sr_pipe_block_read(pipe, (char*)msg.frame.data, msg.frame.timestamp);
//    LOGD("AudioPlayer::onRecvMessage exit\n");
}
