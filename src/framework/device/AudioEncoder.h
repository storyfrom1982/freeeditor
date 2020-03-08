//
// Created by yongge on 20-3-3.
//

#ifndef ANDROID_AUDIOENCODER_H
#define ANDROID_AUDIOENCODER_H

#include <MediaModule.h>

namespace freee {


    class AudioEncoder : public MediaModule {
    public:
        static AudioEncoder* Create(std::string name);
        ~AudioEncoder();

    protected:
    public:
        void FinalClear() override;

    protected:
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessMedia(SmartPkt pkt) override;

        void onMsgRecvEvent(SmartPkt pkt) override;

        void onMsgControl(SmartPkt pkt) override;

    protected:
        AudioEncoder(int mediaType = MediaType_Audio,
                     int mediaNumber = MediaNumber_AudioEncoder,
                     const std::string &mediaName = "AudioEncoder");

    protected:
        int64_t m_startTimestamp = -1;
        size_t m_bufferSize = 0;
        BufferPool *p_bufferPool = nullptr;
    };

}


#endif //ANDROID_AUDIOENCODER_H
