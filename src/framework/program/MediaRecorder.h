//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include "../../message/MessageContext.h"
#include "../../media/AudioSource.h"
#include "../../media/VideoEncoder.h"
#include "../../message/MessageProcessor.h"
#include "../../media/AudioEncoder.h"
#include "../../media/VideoSource.h"
#include "../../media/VideoRenderer.h"
#include "../../media/VideoFilter.h"
#include "../../media/AudioFilter.h"
#include "../../media/MediaStream.h"

namespace freee{

    class MediaRecorder : public MessageChain {

    public:
        MediaRecorder();
        ~MediaRecorder();

    private:
        void FinalClear() override;

    private:
        void onRecvMessage(Message msg) override;

    private:
        void onMsgStartPreview(Message pkt);
        void onMsgStartRecord(Message pkt);
        void onMsgStopRecord(Message pkt);
        void onMsgStopPreview(Message pkt);


    protected:
        json &GetConfig(MessageChain *chain) override;

        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgStart(Message pkt) override;

        void onMsgStop(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

        void onMsgControl(Message pkt) override;

    private:
        bool is_recording;
        bool is_previewing;


        AudioSource *m_audioSource;
        AudioFilter *m_audioFilter;
        AudioEncoder *m_audioEncoder;

        VideoFilter *m_videoFilter;
        VideoSource *m_videoSource;
        VideoRenderer *m_videoRenderer;
        VideoEncoder *m_videoEncoder;

        MediaStream *m_mediaStream;
    };
}



#endif //ANDROID_MEDIARECORD_H
