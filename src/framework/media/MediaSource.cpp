//
// Created by yongge on 20-3-15.
//

#include "MediaSource.h"
#include "ffmpeg/FFmpegMediaSource.h"

extern "C"{

#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

}


using namespace freee;


enum {
    MsgKey_ReadSource = 1000,
};


MediaSource::MediaSource(const std::string &name, int type) : MessageChain(name, type)
{
    StartProcessor();
}

MediaSource::~MediaSource()
{
    StopProcessor();
}

void MediaSource::onReadSource(Message msg)
{

}

void MediaSource::onMsgOpen(Message pkt)
{
    OpenSource(pkt);
    m_status = Status_Opened;
}

void MediaSource::onMsgClose(Message pkt)
{
    CloseSource();
}

void MediaSource::onMsgStart(Message pkt)
{
    if (__set_true(m_isRunning)){
        if (m_outputChainStatus == Status_Opened){
            ReadSource();
        }
    }
}

void MediaSource::onMsgStop(Message pkt)
{
    __set_false(m_isRunning);
}

void MediaSource::onMsgProcessData(Message msg)
{
    MessageChain *chain = m_streamMap[msg.GetFramePtr()->index];
    chain->ProcessData(this, msg);
    ProcessMessage(NewMessage(MsgKey_ReadSource));
}

void MediaSource::onMsgProcessEvent(Message pkt)
{
    switch (pkt.event()){
        case MsgKey_Open:
            m_openedStream ++;
            if (m_openedStream == m_streamCount){
                m_outputChainStatus = Status_Opened;
                if (m_isRunning){
                    ReadSource();
                }
            }
            break;
        case MsgKey_Close:
            m_outputChainStatus = Status_Closed;
            break;
        default:
            break;
    }
}

void MediaSource::onMsgControl(Message pkt)
{
    switch (pkt.key()){
        case MsgKey_ReadSource:
            onMsgReadSource(pkt);
            break;
        default:
            break;
    }
}

MediaSource *MediaSource::Create(std::string url)
{
    return new FFmpegMediaSource();
}

void MediaSource::onMsgReadSource(Message msg)
{
    if (__is_true(m_isRunning)){
//        LOGD("MediaSource::onMsgReadSource");
        ReadSource();
    }
}

json &MediaSource::GetConfig(MessageChain *chain)
{
    m_streamMap[chain->GetStreamId(this)] = chain;
    return m_config[std::to_string(chain->GetStreamId(this))];
}

std::string &MediaSource::GetExtraConfig(MessageChain *chain)
{
    return m_extraConfigMap[chain->GetStreamId(this)];
}
