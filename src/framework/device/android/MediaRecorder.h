//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <MessageContext.h>
#include <AudioSource.h>
#include <VideoEncoder.h>
#include <MessageProcessor.h>
#include <MediaModule.h>
#include <AudioEncoder.h>
#include "../../media/VideoSource.h"
#include "../../media/VideoRenderer.h"
#include "../../media/VideoFilter.h"
#include "../../media/AudioFilter.h"
#include "../../media/MediaStream.h"

namespace freee{

    class MediaRecorder : public MessageContext, MediaChainImpl {

    public:
        MediaRecorder();
        ~MediaRecorder();

    private:
        void FinalClear() override;

    private:
        void onRecvMessage(SmartPkt pkt) override;

    protected:
        void MessageProcess(SmartPkt pkt) override;

    private:
        void StartPreview(SmartPkt pkt);
        void Open(SmartPkt pkt);
        void Close();
        void Start();
        void Stop();
        void StartRecord(SmartPkt pkt);
        void StopRecord();
        void StopPreview();

        void FinalClearVideoChain();

    public:
        void ConnectContext(MessageContext *context) override;
        json &GetConfig(MediaChain *chain) override;

    private:
        int m_status;

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
