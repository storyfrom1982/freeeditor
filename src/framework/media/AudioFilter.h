//
// Created by yongge on 20-3-2.
//

#ifndef ANDROID_AUDIOFILTER_H
#define ANDROID_AUDIOFILTER_H


#include <MediaModule.h>

namespace freee {

    class AudioFilter : public MediaModule {
    public:
        AudioFilter(int mediaType = MediaType_Audio,
                int mediaNumber = MediaNumber_AudioFilter,
                const std::string &mediaName = "AudioFilter");

        void FinalClear() override;

        ~AudioFilter();

    protected:
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessData(SmartPkt pkt) override;

        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(SmartPkt pkt) override;

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



#endif //ANDROID_AUDIOFILTER_H
