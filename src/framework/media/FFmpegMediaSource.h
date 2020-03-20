//
// Created by yongge on 20-3-15.
//

#ifndef ANDROID_FFMPEGMEDIASOURCE_H
#define ANDROID_FFMPEGMEDIASOURCE_H


#include "MediaSource.h"

extern "C"{

#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

}

namespace freee {

    class FFmpegMediaSource : public MediaSource
    {
    public:
        FFmpegMediaSource();
        ~FFmpegMediaSource();

    protected:
        int OpenSource(Message msg) override;

        void CloseSource() override;

        int ReadSource() override;

    public:
        int m_isAbort = 0;

    private:
        AVFormatContext *m_pContext = nullptr;
        AVIOInterruptCB m_interruptCallback = {0};
    };

}


#endif //ANDROID_FFMPEGMEDIASOURCE_H
