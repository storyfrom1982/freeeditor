//
// Created by yongge on 20-3-18.
//

#include "FFmpegAudioDecoder.h"


using namespace freee;

FFmpegAudioDecoder::FFmpegAudioDecoder()
{

}

FFmpegAudioDecoder::~FFmpegAudioDecoder()
{
    StopProcessor();
}

int FFmpegAudioDecoder::OpenDecoder()
{
    return 0;
}

void FFmpegAudioDecoder::CloseDecoder()
{

}

int FFmpegAudioDecoder::DecodeAudio(Message msg)
{
    LOGD("FFmpegAudioDecoder::DecodeAudio data size %lu\n", msg.GetMsgLength());
    return 0;
}
