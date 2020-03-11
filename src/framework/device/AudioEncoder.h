//
// Created by yongge on 20-3-3.
//

#ifndef ANDROID_AUDIOENCODER_H
#define ANDROID_AUDIOENCODER_H

#include <MediaModule.h>

namespace freee {


    class AudioEncoder : public MessageChain {
    public:
        static AudioEncoder* Create(std::string name);
        ~AudioEncoder();

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
        AudioEncoder(const std::string &mediaName = "AudioEncoder",
                int mediaType = MediaType_Audio);

    protected:
        int64_t m_startTimestamp = -1;
        size_t m_bufferSize = 0;
        BufferPool *p_bufferPool = nullptr;
    };

}


#endif //ANDROID_AUDIOENCODER_H
