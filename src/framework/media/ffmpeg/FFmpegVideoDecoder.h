//
// Created by yongge on 20-3-18.
//

#ifndef ANDROID_FFMPEGVIDEODECODER_H
#define ANDROID_FFMPEGVIDEODECODER_H

#include "../VideoDecoder.h"

extern "C"{

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>

}

namespace freee {


    class FFmpegVideoDecoder : public VideoDecoder
    {
    public:
        FFmpegVideoDecoder();
        ~FFmpegVideoDecoder();
        static int get_buffer2(struct AVCodecContext *s, AVFrame *frame, int flags);

    protected:
        int OpenDecoder() override;

        void CloseDecoder() override;

        int DecodeVideo(Message msg) override;

        int decode(AVCodecContext *avctx, AVFrame *picture,
                                       int *got_picture_ptr,
                                       const AVPacket *avpkt);

    private:
        void GetVideoBuffer(AVFrame *frame);
        void CreateBufferPool(AVFrame *frame);

    private:
        size_t  m_bufferSize = 0;
        MessagePool *m_pBufferPool = nullptr;
        int m_planeCount = 0;
        int m_planeSize[4] = {0};
        std::map<void*, Message> frameMap;
        AVCodecContext *m_pCodecContext = nullptr;
    };

}


#endif //ANDROID_FFMPEGVIDEODECODER_H
