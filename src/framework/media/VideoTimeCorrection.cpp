//
// Created by yongge on 20-5-17.
//

#include <unistd.h>
#include "VideoTimeCorrection.h"


using namespace freee;

VideoTimeCorrection::VideoTimeCorrection(const std::string &name) : MediaPlugin(name)
{
    StartProcessor();
}

VideoTimeCorrection::~VideoTimeCorrection()
{
    StopProcessor();
}

void VideoTimeCorrection::onMsgOpen(Message msg)
{
    m_config = static_cast<MessageChain *>(msg.obj())->GetConfig(this);
    MessageChain::onMsgOpen(msg);
}

void VideoTimeCorrection::onMsgClose(Message msg)
{
    MessageChain::onMsgClose(msg);
}

void VideoTimeCorrection::onMsgProcessData(Message msg)
{
    if (m_startTime == 0){
        m_startTime = sr_time_begin();
    }

//    LOGD("VideoTimeCorrection m_currentTime = %lld pts = %lld delay time %lld\n", m_currentTime, msg.msgFrame()->timestamp * 1000, m_delayTime);

    m_currentTime = sr_time_passed(m_startTime);
    m_delayTime = msg.msgFrame()->timestamp * 1000 - m_currentTime;

    if (m_delayTime > 0){
        MessageChain::onMsgProcessData(msg);
        m_currentTime = sr_time_passed(m_startTime);
        m_delayTime = msg.msgFrame()->timestamp * 1000 - m_currentTime;
        usleep(m_delayTime);
    }
}
