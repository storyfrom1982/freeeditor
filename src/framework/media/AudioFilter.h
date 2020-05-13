//
// Created by yongge on 20-3-2.
//

#ifndef ANDROID_AUDIOFILTER_H
#define ANDROID_AUDIOFILTER_H


#include "../message/MessageChain.h"

namespace freee {

    class AudioFilter : public MessageChain {
    public:
        AudioFilter(const std::string &name = "AudioFilter");

        void FinalClear() override;

        virtual ~AudioFilter();

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        int OpenMedia() override;

        void CloseMedia() override;

        int ProcessMedia(Message pkt) override;

    private:
        int m_srcSampleRate;
        int m_srcChannelCount;
        int m_srcBytePerSample;
        int m_codecSampleRate;
        int m_codecChannelCount;
        int m_codecBytePerSample;
        int m_codecSamplePerFrame;
        uint32_t m_srcVideoFormat, m_codecVideoFormat;

        size_t m_bufferSize;
        MessagePool *p_bufferPool = nullptr;
    };

}



#endif //ANDROID_AUDIOFILTER_H
