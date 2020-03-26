//
// Created by yongge on 20-3-22.
//

#ifndef ANDROID_AUDIOPLAYER_H
#define ANDROID_AUDIOPLAYER_H

#include "../message/MessageChain.h"

namespace freee {


    class AudioPlayer : public MessageChain
    {
    public:

        AudioPlayer(const std::string name = "AudioPlayer");

        virtual ~AudioPlayer();

        void Open(MessageChain *chain) override;

        void Close(MessageChain *chain) override;

        void Start(MessageChain *chain) override;

        void Stop(MessageChain *chain) override;

        void ProcessData(MessageChain *chain, Message msg) override;

    protected:
        void onRecvMessage(Message msg) override;

    private:

        sr_pipe_t *pipe = nullptr;
    };

}


#endif //ANDROID_AUDIOPLAYER_H
