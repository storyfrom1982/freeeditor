//
// Created by yongge on 20-3-15.
//

#include "MediaPlayer.h"


using namespace freee;

MediaPlayer::MediaPlayer(const std::string &name, int type)
    : MessageChain(name, type)
{
    StartProcessor();
}

MediaPlayer::~MediaPlayer()
{
    StopProcessor();
}

void MediaPlayer::onMsgOpen(Message pkt)
{
    LOGD("MediaPlayer::onMsgOpen %s\n", pkt.GetString().c_str());
}

void MediaPlayer::onMsgClose(Message pkt)
{
    MessageChain::onMsgClose(pkt);
}

void MediaPlayer::onMsgStart(Message pkt)
{
    MessageChain::onMsgStart(pkt);
}

void MediaPlayer::onMsgStop(Message pkt)
{
    MessageChain::onMsgStop(pkt);
}

void MediaPlayer::onMsgControl(Message pkt)
{
    MessageChain::onMsgControl(pkt);
}

void MediaPlayer::onRecvMessage(Message msg)
{
    ProcessMessage(msg);
}
