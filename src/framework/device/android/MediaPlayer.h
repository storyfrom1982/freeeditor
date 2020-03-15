//
// Created by yongge on 20-3-15.
//

#ifndef ANDROID_MEDIAPLAYER_H
#define ANDROID_MEDIAPLAYER_H


#include <MessageChain.h>

namespace freee {


    class MediaPlayer : public MessageChain
    {
    public:
        MediaPlayer(const std::string &name = "MediaPlayer", int type = MediaType_All);
        ~MediaPlayer();

    protected:
        void onRecvMessage(Message msg) override;

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgStart(Message pkt) override;

        void onMsgStop(Message pkt) override;

        void onMsgControl(Message pkt) override;
    };


}


#endif //ANDROID_MEDIAPLAYER_H
