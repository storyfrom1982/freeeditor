//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_FFMPEGAUDIODECODER_H
#define ANDROID_FFMPEGAUDIODECODER_H


#include "../AudioDecoder.h"

extern "C"{

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>

}


namespace freee {


    class FFmpegAudioDecoder : public AudioDecoder
    {
    public:
        FFmpegAudioDecoder();
        ~FFmpegAudioDecoder();
        static int get_buffer2(struct AVCodecContext *s, AVFrame *frame, int flags);

    protected:
        int OpenDecoder() override;

        void CloseDecoder() override;

        int DecodeAudio(Message msg) override;

    private:
        size_t  m_bufferSize = 0;
        MessagePool *m_pBufferPool = nullptr;
        int m_planeCount = 0;
        int m_planeSize[4] = {0};
        std::map<void*, Message> frameMap;
        AVCodecContext *m_pCodecContext = nullptr;
    };

}


#endif //ANDROID_FFMPEGAUDIODECODER_H
