//
// Created by yongge on 20-3-6.
//

#include <unistd.h>
#include "FileMediaStream.h"


using namespace freee;

FileMediaStream::FileMediaStream() {
    m_pContext = nullptr;
    m_Streams = std::vector<AVStream*>(3);
}

FileMediaStream::~FileMediaStream() {

}

void FileMediaStream::onMsgConnectStream(Message pkt) {
    av_register_all();
    std::string url = pkt.GetString();
    if (avformat_alloc_output_context2(&m_pContext, NULL, NULL, url.c_str()) < 0) {
        LOGD("[FileMediaStream] ConnectStream failed to %s\n", url.c_str());
        return;
    }

//    m_pContext->flags = AV_CODEC_FLAG_GLOBAL_HEADER;
//    m_pContext->ctx_flags = AV_CODEC_FLAG_GLOBAL_HEADER;

    int ret = avio_open(&m_pContext->pb, url.c_str(), AVIO_FLAG_WRITE);
    if (NULL == m_pContext->pb){
        //open file failed callback
        char buffer[1024];
        av_strerror(ret, buffer, sizeof(buffer) - 1);
        LOGD("[FileMediaStream] open %s failed ret = %d error=%s\n",
             url.c_str(), ret, buffer);
    }
    LOGD("[FileMediaStream] ConnectStream url %s\n", url.c_str());
}

void FileMediaStream::onMsgDisconnectStream() {
    if (m_pContext){
        if (m_status == Status_Opened && m_pContext->pb != NULL) {
            av_write_trailer(m_pContext);
        }
        avio_close(m_pContext->pb);
        m_pContext->pb = NULL;
    }
}

void FileMediaStream::onMsgOpen(Message pkt) {
    MessageChain *chain = static_cast<MessageChain *>(pkt.GetPtr());
    if (m_chainToStream[chain] == nullptr){
        LOGD("FileMediaStream::onMsgOpen type[%s] extraConfig %lu\n", chain->GetName(this).c_str(), chain->GetExtraConfig(this).size());
        if (chain->GetType(this) == MediaType_Audio){
            addAudioStream(chain);
        }else if (chain->GetType(this) == MediaType_Video){
            addVideoStream(chain);
        }

        if (m_pContext->nb_streams == m_chainToStream.size()){
            AVDictionary* option = NULL;
            LOGD("FileMediaStream::onMsgOpen avformat_write_header\n");
            int result = avformat_write_header(m_pContext, &option);
            if (result < 0){
                char buffer[1024];
                av_strerror(result, buffer, sizeof(buffer) - 1);
                LOGD("[FileMediaStream] avformat_write_header failed %d %s\n", result, buffer);
            }

            m_status = Status_Opened;
            usleep(200000);
            Message event(MsgKey_ProcessEvent);
            event.SetEvent(MsgKey_Open);
            MessageChain::onMsgProcessEvent(event);
        }
    }
}

void FileMediaStream::onMsgClose(Message pkt) {
    MediaStream::onMsgClose(pkt);
}

void FileMediaStream::onMsgProcessData(Message pkt) {
    MessageChain *chain = static_cast<MessageChain *>(pkt.GetPtr());

    AVStream* pStream = nullptr;
    {
        AutoLock lock(m_lockInputChain);
        pStream = (AVStream*)m_chainToStream[chain];
        if (pStream == nullptr){
            return;
        }
    }

    AVPacket             avpkt;
    ::av_init_packet(&avpkt);
    avpkt.stream_index = pStream->index;
    avpkt.data = pkt.frame.data;
    avpkt.size = pkt.frame.size;
    avpkt.dts = pkt.frame.timestamp*pStream->time_base.den / (1000 * pStream->time_base.num);
    avpkt.pts = avpkt.dts;

    avpkt.flags = (pkt.frame.flag & PktFlag_KeyFrame) ? AV_PKT_FLAG_KEY : 0;

    if (pkt.frame.type == MediaType_Audio){
//        LOGD("FileMediaStream: audio timebase[%d/%d] pts=%lld  id=%lld\n", pStream->time_base.den,
//             pStream->time_base.num, pkt.frame.timestamp, avpkt.pts);
    }else if (pkt.frame.type == MediaType_Video){
        LOGD("FileMediaStream: video flag %d\n", avpkt.flags);
//        LOGD("FileMediaStream: video timebase[%d/%d] pts=%lld  id=%lld\n", pStream->time_base.den,
//             pStream->time_base.num, pkt.frame.timestamp, avpkt.pts);
    }

    if (m_pContext->nb_streams < m_chainToStream.size()){
        return;
    }

    if (m_pContext != NULL && m_pContext->pb != NULL){
        int result = av_write_frame(m_pContext, &avpkt);
        if (result < 0){
            char buffer[1024];
            av_strerror(result, buffer, sizeof(buffer) - 1);
            if (pkt.frame.type == MediaType_Audio){
                LOGD("[FileMediaStream] av_write_frame audio %d, ret=%d error=%s\n",
                     pStream->index, result,buffer);
            }else if (pkt.frame.type == MediaType_Video){
                LOGD("[FileMediaStream] av_write_frame video %d, ret=%d error=%s\n",
                     pStream->index, result,buffer);
            }
        }
    }
}

AVStream *FileMediaStream::addAudioStream(MessageChain *chain) {

    json cfg = chain->GetConfig(this);

    AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_AAC);

    AVStream* avStream = avformat_new_stream(m_pContext, NULL);
    avStream->codecpar->codec_id = codec->id;
    avStream->codecpar->codec_type = codec->type;

#if LIBAVFORMAT_VERSION_MAJOR >= 57
    AVCodecParameters *codecpar = avStream->codecpar;
#else
    AVCodecContext*codecpar = avStream->codec;
#endif

    m_pContext->video_codec_id = codecpar->codec_id;
    int sampleRate = cfg["codecBitRate"];
    codecpar->sample_rate = cfg["codecSampleRate"];
    codecpar->channels = cfg["codecChannelCount"];
    codecpar->bit_rate = cfg["codecBitRate"];
//    avStream->time_base.den = 1;
//    avStream->time_base.num = 1;
    avStream->time_base = (AVRational){ 1, sampleRate };

    std::string extraConfig = chain->GetExtraConfig(this);
    if (extraConfig.size() > 0){
        codecpar->extradata_size = extraConfig.size();
        codecpar->extradata = (uint8_t*)av_malloc(codecpar->extradata_size);
        memcpy(codecpar->extradata,
               extraConfig.c_str(), extraConfig.size());
    }

    m_chainToStream[chain] = avStream;

    return avStream;
}

AVStream *FileMediaStream::addVideoStream(MessageChain *chain) {

    json cfg = chain->GetConfig(this);

    AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);

    AVStream* avStream = avformat_new_stream(m_pContext, NULL);
    avStream->codecpar->codec_id = codec->id;
    avStream->codecpar->codec_type = codec->type;

#if LIBAVFORMAT_VERSION_MAJOR >= 57
    AVCodecParameters *codecpar = avStream->codecpar;
#else
    AVCodecContext*codecpar = avStream->codec;
#endif

    m_pContext->video_codec_id = codecpar->codec_id;
    int fps = (int)((float)cfg["codecFPS"]);
    codecpar->width = cfg["codecWidth"];
    codecpar->height = cfg["codecHeight"];
//    avStream->avg_frame_rate.num = 1;
//    avStream->avg_frame_rate.den = 1000;
    avStream->avg_frame_rate = (AVRational){ 1000, fps * 1000 };
    codecpar->bit_rate = cfg["codecBitRate"];
    avStream->time_base = (AVRational){ 1, 1 };
//    avStream->time_base.den = 1;
//    avStream->time_base.num = 1;

    std::string extraConfig = chain->GetExtraConfig(this);
    if (extraConfig.size() > 0){
        codecpar->extradata_size = extraConfig.size();
        codecpar->extradata = (uint8_t*)av_malloc(codecpar->extradata_size);
        memcpy(codecpar->extradata,
               extraConfig.c_str(), extraConfig.size());
    }

    m_chainToStream[chain] = avStream;

    return avStream;
}
