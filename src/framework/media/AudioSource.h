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

        void ProcessData(MessageChain *chain, Message msg) override;

        virtual void onRecvMessage(Message msg) override;

        virtual Message onRequestMessage(int key) override;


    private:
        void onRecvEvent(Message msg);
        void UpdateConfig(Message msg);


    private:
        int64_t m_startTIme = 0;
        int64_t m_totalSamples = 0;
        size_t m_srcSampleRate = 0;
        size_t m_srcChannelCount = 0;
        size_t m_srcBytePerSample = 0;
        size_t m_srcSamplesPerFrame = 0;
        size_t m_codecSampleRate = 0;
        size_t m_codecChannelCount = 0;
        size_t m_codecBytePerSample = 0;
        size_t m_codecSamplesPerFrame = 0;

        sr_pipe_t *p_pipe = nullptr;

        size_t m_bufferSize;
        MessagePool *p_bufferPool = nullptr;

    };


}



#endif //ANDROID_AUDIOSOURCE_H
