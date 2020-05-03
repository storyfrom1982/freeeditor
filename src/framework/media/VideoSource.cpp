//
// Created by yongge on 20-2-20.
//

#include <MediaContext.h>
#include "VideoSource.h"

using namespace std;
using namespace freee;

VideoSource::VideoSource(MessageContext *context)
    : MessageChain("VideoSource") {
    m_type = MediaType_Video;
    m_status = Status_Closed;
    if (context == nullptr){
        context = MediaContext::Instance()->ConnectCamera();
    }
    ConnectContext(context);
}

VideoSource::~VideoSource() {
    DisconnectContext();
    MediaContext::Instance()->DisconnectCamera();
    FinalClear();
}

void VideoSource::Open(MessageChain *chain) {
    LOGD("VideoSource::Open enter\n");
    m_config = chain->GetConfig(this);
    SendMessage(NewMessage(MsgKey_Open, m_config.dump()));
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MessageChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(NewMessage(MsgKey_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MessageChain *chain) {
    LOGD("VideoSource::Start enter\n");
    SendMessage(NewMessage(MsgKey_Start));
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop(MessageChain *chain) {
    LOGD("VideoSource::Stop enter\n");
    SendMessage(NewMessage(MsgKey_Stop));
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::ProcessData(MessageChain *chain, Message msg) {

    if (msg.GetDataPtr() != msg.GetBufferPtr()){
        msg = NewMessage(MsgKey_ProcessData, msg.GetDataPtr(), msg.GetDataSize());
    }

    if (m_startTime == 0){
        m_startTime = sr_time_begin();
        msg.GetFramePtr()->timestamp = 0;
    }else {
        msg.GetFramePtr()->timestamp = sr_time_passed(m_startTime);
    }

    libyuv_set_format(msg.GetFramePtr(), msg.GetDataPtr(),
                      m_srcWidth, m_srcHeight, m_srcImageFormat);

    MessageChain::onMsgProcessData(msg);

//    if (m_srcImageFormat != m_codecImageFormat
//        || m_srcWidth != m_codecWidth
//        || m_srcHeight != m_codecHeight){
//        if (p_bufferPool){
//            Message y420 = p_bufferPool->NewMessage(MsgKey_ProcessData);
//            if (m_startTime == 0){
//                m_startTime = sr_time_begin();
//                y420.GetFramePtr()->timestamp = 0;
//            }else {
//                y420.GetFramePtr()->timestamp = sr_time_passed(m_startTime);
//            }
//            libyuv_set_format(y420.GetFramePtr(), y420.GetDataPtr(), m_codecWidth,
//                              m_codecHeight, m_codecImageFormat);
//            libyuv_convert_to_yuv420p(msg.GetFramePtr(), y420.GetFramePtr(), m_srcRotation);
//            MessageChain::onMsgProcessData(y420);
//        }
//    }else {
//        MessageChain::onMsgProcessData(msg);
//    }
}

void VideoSource::onRecvMessage(Message msg) {
    switch (msg.key()){
        case MsgKey_ProcessData:
            ProcessData(this, msg);
            break;
        case MsgKey_ProcessEvent:
            onRecvEvent(msg);
            break;
        default:
            break;
    }
}

void VideoSource::UpdateMediaConfig(Message msg) {
//    LOGD("VideoSource::UpdateMediaConfig >> %s\n", pkt.msg.json);
    m_config = json::parse(msg.GetString());
    m_srcWidth = m_config[CFG_SRC_WIDTH];
    m_srcHeight = m_config[CFG_SRC_HEIGHT];
    m_srcRotation = m_config[CFG_SRC_ROTATION];
    m_codecWidth = m_config[CFG_CODEC_WIDTH];
    m_codecHeight = m_config[CFG_CODEC_HEIGHT];
    std::string srcFormat = m_config[CFG_SRC_IMAGE_FORMAT];
    std::string codecFormat = m_config[CFG_CODEC_IMAGE_FORMAT];
    m_srcImageFormat = libyuv_convert_fourcc(srcFormat.c_str());
    m_codecImageFormat = libyuv_convert_fourcc(codecFormat.c_str());
    m_bufferSize = m_codecWidth * m_codecHeight / 2 * 3U;
    p_bufferPool = new MessagePool(GetName() + "FramePool", m_bufferSize, 10, 64 ,0, 0);

    MessageChain::onMsgOpen(msg);
}

void VideoSource::FinalClear() {
    if (p_bufferPool){
        delete p_bufferPool;
        p_bufferPool = nullptr;
    }
}

void VideoSource::onRecvEvent(Message msg)
{
    switch (msg.event()){
        case Status_Opened:
            m_status = Status_Opened;
            msg.GetMessagePtr()->key = MsgKey_Open;
            UpdateMediaConfig(msg);
            break;
        case Status_Closed:
            m_status = Status_Closed;
            msg.GetMessagePtr()->key = MsgKey_Close;
            MessageChain::onMsgClose(msg);
            FinalClear();
            break;
        case Status_Started:
            m_status = Status_Started;
            break;
        case Status_Stopped:
            m_status = Status_Stopped;
            break;
        case Status_Error:
            m_status = Status_Stopped;
            LOGE("VideoSource status error: %s\n", msg.GetString().c_str());
            break;
        default:
            break;
    }
}
