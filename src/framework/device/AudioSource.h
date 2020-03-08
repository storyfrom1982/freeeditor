//
// Created by yongge on 20-2-15.
//

#ifndef ANDROID_AUDIOSOURCE_H
#define ANDROID_AUDIOSOURCE_H


#include <MessageContext.h>
#include <MediaChainImpl.h>


namespace freee {


    class AudioSource : public MediaChainImpl {
    public:
        void FinalClear() override;

    public:

        AudioSource(int mediaType = MediaType_Audio,
                int mediaNumber = MediaNumber_AudioSource,
                std::string mediaName = "AudioSource");

        ~AudioSource();

        void Open(MediaChain *chain) override;

        void Close(MediaChain *chain) override;

        void Start(MediaChain *chain) override;

        void Stop(MediaChain *chain) override;

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override;

        virtual void onRecvMessage(SmartPkt pkt) override;

        virtual SmartPkt onObtainMessage(int key) override;


    private:
        void UpdateConfig(SmartPkt pkt);


    private:
        int m_status;
        int m_srcSampleRate;
        int m_srcChannelCount;
        int m_srcBytePerSample;
        int m_codecSampleRate;
        int m_codecChannelCount;
        int m_codecBytePerSample;
        int m_codecSamplePerFrame;
        uint32_t m_srcVideoFormat, m_codecVideoFormat;

        size_t m_bufferSize;
        BufferPool *p_bufferPool = nullptr;

    };


}



#endif //ANDROID_AUDIOSOURCE_H
