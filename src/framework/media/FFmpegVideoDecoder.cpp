//
// Created by yongge on 20-3-18.
//

#include "FFmpegVideoDecoder.h"


using namespace freee;

FFmpegVideoDecoder::FFmpegVideoDecoder()
{

}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
    StopProcessor();
}

int FFmpegVideoDecoder::OpenDecoder()
{
    return 0;
}

void FFmpegVideoDecoder::CloseDecoder()
{

}

int FFmpegVideoDecoder::DecodeVideo(Message msg)
{
    LOGD("FFmpegVideoDecoder::DecodeVideo data size %lu\n", msg.GetMsgLength());
    return 0;
}
