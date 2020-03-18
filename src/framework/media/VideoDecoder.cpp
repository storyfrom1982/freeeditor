//
// Created by yongge on 20-3-18.
//

#include "VideoDecoder.h"
#include "FFmpegVideoDecoder.h"


using namespace freee;

VideoDecoder::VideoDecoder(const std::string &name, int type) : MessageChain(name, type)
{
    StartProcessor();
}

VideoDecoder::~VideoDecoder()
{
    StopProcessor();
}

void VideoDecoder::onMsgOpen(Message msg)
{
    MessageChain *chain = static_cast<MessageChain *>(msg.GetPtr());
    m_config = chain->GetConfig(this);
    LOGD("VideoDecoder::onMsgOpen config: %s\n", m_config.dump().c_str());
    OpenDecoder();
    Message event(MsgKey_ProcessEvent);
    event.SetEvent(MsgKey_Open);
    MessageChain::onMsgProcessEvent(event);
}

void VideoDecoder::onMsgClose(Message msg)
{
    CloseDecoder();
}

void VideoDecoder::onMsgProcessData(Message msg)
{
    DecodeVideo(msg);
}

void VideoDecoder::onMsgProcessEvent(Message msg)
{

}

void VideoDecoder::onMsgControl(Message msg)
{

}

VideoDecoder *VideoDecoder::Create(std::string name)
{
    return new FFmpegVideoDecoder();
}
