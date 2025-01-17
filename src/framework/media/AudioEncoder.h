//
// Created by yongge on 20-3-3.
//

#ifndef ANDROID_AUDIOENCODER_H
#define ANDROID_AUDIOENCODER_H


#include "../message/MessageChain.h"
#include "MediaPlugin.h"

namespace freee {


    class AudioEncoder : public MediaPlugin {
    public:
        static AudioEncoder* Create(std::string name);
        virtual ~AudioEncoder();

    protected:
    public:
        void FinalClear() override;

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

        void onMsgControl(Message pkt) override;

    protected:
        AudioEncoder(const std::string name);

    protected:
        int64_t m_startTimestamp = -1;
        size_t m_bufferSize = 0;
        MessagePool *p_bufferPool = nullptr;
    };

}


#endif //ANDROID_AUDIOENCODER_H
