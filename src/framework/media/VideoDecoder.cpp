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
    LOGD("VideoDecoder::onMsgOpen =========================================== enter\n");
    if (m_status == Status_Opened){
        return;
    }
    MessageChain *chain = static_cast<MessageChain *>(msg.GetObject());
    m_config = chain->GetConfig(this);
    m_extraConfig = chain->GetExtraConfig(this);
    LOGD("VideoDecoder::onMsgOpen config: %s\n", m_config.dump().c_str());
    OpenDecoder();
    MessageChain::onMsgOpen(msg);
    MessageChain::onMsgProcessEvent(NewFrameMessage(MsgKey_ProcessEvent, MsgKey_Open));
    m_status = Status_Opened;
    LOGD("VideoDecoder::onMsgOpen =========================================== exit\n");
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
