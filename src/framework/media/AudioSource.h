//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOSOURCE_H
#define ANDROID_AUDIOSOURCE_H


#include "../message/MessageContext.h"
#include "../message/MessageChain.h"


namespace freee {


    class AudioSource : public MessageChain {
    public:
        void FinalClear() override;

    public:

        AudioSource(const std::string name = "AudioSource");

        virtual ~AudioSource();

        void Open(MessageChain *chain) override;

        void Close(MessageChain *chain) override;

        void Start(MessageChain *chain) override;

        void Stop(MessageChain *chain) override;

        void ProcessData(MessageChain *chain, Message pkt) override;

        virtual void onRecvMessage(Message msg) override;

        virtual Message onRequestMessage(int key) override;


    private:
        void onRecvEvent(Message msg);
        void UpdateConfig(Message msg);


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



#endif //ANDROID_AUDIOSOURCE_H
