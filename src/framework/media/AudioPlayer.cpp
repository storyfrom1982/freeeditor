//
// Created by yongge on 20-3-22.
//

#include <MediaContext.h>
#include "AudioPlayer.h"

#ifdef __cplusplus
extern "C" {
#endif

# include <samplerate.h>

#ifdef __cplusplus
}
#endif


using namespace freee;

AudioPlayer::AudioPlayer(const std::string name) : MessageChain(name)
{
    m_type = MediaType_Audio;
    MessageContext *context = MediaContext::Instance()->ConnectSpeaker();
    ConnectContext(context);
}

AudioPlayer::~AudioPlayer()
{
    DisconnectContext();
    MediaContext::Instance()->DisconnectSpeaker();
    sr_pipe_release(&pipe);
}

void AudioPlayer::Open(MessageChain *chain)
{
    m_config = chain->GetConfig(this);
    pipe = sr_pipe_create(1 << 18);
    Message msg = NewMessage(MsgKey_Open, m_config.dump());
    SendMessage(msg);
    SendMessage(NewMessage(MsgKey_Start));
}

void AudioPlayer::Close(MessageChain *chain)
{
    sr_pipe_stop(pipe);
    SendMessage(NewMessage(MsgKey_Stop));
    SendMessage(NewMessage(MsgKey_Close));
}

void AudioPlayer::Start(MessageChain *chain)
{
    SendMessage(NewMessage(MsgKey_Start));
}

void AudioPlayer::Stop(MessageChain *chain)
{
    SendMessage(NewMessage(MsgKey_Stop));
}

void AudioPlayer::ProcessData(MessageChain *chain, Message msg)
{
    char tmp[2048];
//    short *pcm = (short*)&tmp;
//    for (int j=0; j<1024; j++){
//        for (int c=0 ;c< 1; c++){
//            float *f = (float *) msg.data();
//            float t = f[j];
//            if (t<-1.0f) t=-1.0f;
//            else if (t>1.0f) t=1.0f;
//            *pcm++ = t*32767.0f;
//        }
//    }
    src_float_to_short_array((float *) msg.data(), (short *) tmp, 1024);
    sr_pipe_block_write(pipe, tmp, 2048);
}

void AudioPlayer::onRecvMessage(Message msg)
{
//    LOGD("AudioPlayer::onRecvMessage %lld\n", msg.frame.timestamp);
    if (sr_pipe_readable(pipe) < msg.size()){
        return;
    }
    sr_pipe_block_read(pipe, (char*) msg.data(), msg.size());
//    LOGD("AudioPlayer::onRecvMessage exit\n");
}
