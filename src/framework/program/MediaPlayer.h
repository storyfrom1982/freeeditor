//
// Created by yongge on 20-3-15.
//

#ifndef ANDROID_MEDIAPLAYER_H
#define ANDROID_MEDIAPLAYER_H


#include "../../message/MessageChain.h"
#include "../../media/MediaSource.h"
#include "../../media/VideoDecoder.h"
#include "../../media/AudioDecoder.h"
#include "../../media/VideoRenderer.h"
#include "../../media/AudioPlayer.h"

namespace freee {


    class MediaPlayer : public MessageChain
    {
    public:
        MediaPlayer(const std::string &name = "MediaPlayer", int type = MediaType_All);
        ~MediaPlayer();

    protected:
        void onRecvMessage(Message msg) override;

    protected:
        void onMsgOpen(Message msg) override;

        void onMsgClose(Message pkt) override;

        void onMsgStart(Message pkt) override;

        void onMsgStop(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

        void onMsgControl(Message pkt) override;

    private:
        MediaSource *m_pMediaSource = nullptr;
        VideoDecoder *m_pVideoDeocder = nullptr;
        AudioDecoder *m_pAudioDecoder = nullptr;
        VideoRenderer *m_pVideoRenderer = nullptr;
        AudioPlayer *m_pAudioPlayer = nullptr;
    };


}


#endif //ANDROID_MEDIAPLAYER_H
