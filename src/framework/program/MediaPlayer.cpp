//
// Created by yongge on 20-3-15.
//

#include "MediaPlayer.h"

#include <libavcodec/avcodec.h>


enum {
    MsgKey_Control_SetWindow = 1001,
    MsgKey_Control_Pause = 1002,
};


using namespace freee;

MediaPlayer::MediaPlayer(std::string name) : MessageChain(name)
{
    StartProcessor();
}

MediaPlayer::~MediaPlayer()
{
    StopProcessor();
}

void MediaPlayer::onMsgOpen(Message msg)
{
    LOGD("MediaPlayer::onMsgOpen %s\n", msg.getString().c_str());
    m_pMediaSource = MediaSource::Create("ffmpeg");
    m_pMediaSource->SetEventListener(this);
    m_pMediaSource->Open(this);
    m_pVideoCorrection = new VideoTimeCorrection();
    m_pVideoRenderer = new VideoRenderer();
    m_pAudioPlayer = new AudioPlayer();
}

void MediaPlayer::onMsgClose(Message pkt)
{
    if (m_pMediaSource){
        m_pMediaSource->Close(this);
        delete m_pMediaSource;
        m_pMediaSource = nullptr;
    }
    if (m_pAudioDecoder){
        m_pAudioDecoder->Close(this);
        delete m_pAudioDecoder;
        m_pAudioDecoder = nullptr;
    }
    if (m_pVideoDeocder){
        m_pVideoDeocder->Close(this);
        delete m_pVideoDeocder;
        m_pVideoDeocder = nullptr;
    }
    if (m_pVideoCorrection){
        m_pVideoCorrection->Close(this);
        delete m_pVideoCorrection;
        m_pVideoCorrection = nullptr;
    }
    if (m_pVideoRenderer){
        m_pVideoRenderer->Close(this);
        delete m_pVideoRenderer;
        m_pVideoRenderer = nullptr;
    }
    if (m_pAudioPlayer){
        m_pAudioPlayer->Close(this);
        delete m_pAudioPlayer;
        m_pAudioPlayer = nullptr;
    }
}

void MediaPlayer::onMsgStart(Message pkt)
{
    m_pMediaSource->Start(this);
}

void MediaPlayer::onMsgStop(Message pkt)
{
    m_pMediaSource->Stop(this);
}

void MediaPlayer::onMsgControl(Message pkt)
{
    switch (pkt.key()){
        case MsgKey_Control_SetWindow:
            m_pVideoRenderer->SetVideoWindow(pkt.obj());
            break;
        default:
            break;
    }
}

void MediaPlayer::onRecvMessage(Message msg)
{
    ProcessMessage(msg);
}

void MediaPlayer::onMsgProcessEvent(Message pkt)
{
    if (pkt.obj() == m_pMediaSource){
//        LOGD("Open stream config: %s\n", pkt.getString().c_str());
        json cfg = json::parse(pkt.getString());
        if (cfg[CFG_TYPE] == CFG_VIDEO){
            m_pVideoDeocder = VideoDecoder::Create(cfg[CFG_CODEC_NAME]);
            m_pVideoDeocder->SetStreamId(cfg[CFG_CODEC_STREAM_ID]);
            m_pVideoDeocder->AddOutput(m_pVideoCorrection);
            m_pVideoCorrection->AddOutput(m_pVideoRenderer);
            m_pMediaSource->AddOutput(m_pVideoDeocder);
        }else if (cfg[CFG_TYPE] == CFG_AUDIO){
            m_pAudioDecoder = AudioDecoder::Create(cfg[CFG_CODEC_NAME]);
            m_pAudioDecoder->SetStreamId(cfg[CFG_CODEC_STREAM_ID]);
            m_pAudioDecoder->AddOutput(m_pAudioPlayer);
            m_pMediaSource->AddOutput(m_pAudioDecoder);
        }
    }
}
