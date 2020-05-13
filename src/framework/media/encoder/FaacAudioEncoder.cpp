//
// Created by yongge on 20-2-15.
//

#include "FaacAudioEncoder.h"


using namespace freee;

FaacAudioEncoder::FaacAudioEncoder() : AudioEncoder("FaacAudioEncoder") {

}

FaacAudioEncoder::~FaacAudioEncoder() {
    StopProcessor();
}


int FaacAudioEncoder::OpenMedia(MediaPlugin *plugin) {

    int bitRate = m_config[CFG_CODEC_BITRATE];
    int sampleRate = m_config[CFG_CODEC_SAMPLE_RATE];
    int targetCh = m_config[CFG_CODEC_CHANNEL_COUNT];
    m_inputSamples = m_config[CFG_CODEC_SAMPLES_PER_FRAME];

    int aot = AOT_AAC_LC;// AOT_AAC_LC;// AOT_ER_AAC_ELD;
    int afterburner = 1;
    int eld_sbr =  1;

    if (aacEncOpen(&m_pHandle, 0, targetCh) != AACENC_OK) {
        LOGD("[FaacAudioEncoder] Unable to EncOpen\n");
        return -1;
    }

    if (aacEncoder_SetParam(m_pHandle, AACENC_AOT, aot) != AACENC_OK) {
        LOGD( "[FaacAudioEncoder] Unable to set the AOT\n");
        return 1;
    }

    if ( eld_sbr) {
        if (aacEncoder_SetParam(m_pHandle, AACENC_SBR_MODE, 1) != AACENC_OK) {
            LOGD("Unable to set SBR mode for ELD\n");
            return 1;
        }
    }
    if (aacEncoder_SetParam(m_pHandle, AACENC_SAMPLERATE, sampleRate) != AACENC_OK) {
        LOGD("Unable to set the AOT\n");
        return 1;
    }

    int mode = targetCh == 1 ? MODE_1 : MODE_2;
    if (aacEncoder_SetParam(m_pHandle, AACENC_CHANNELMODE, mode) != AACENC_OK) {
        LOGD("Unable to set the channel mode\n");
        return 1;
    }
    if (aacEncoder_SetParam(m_pHandle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
        LOGD("Unable to set the wav channel order\n");
        return 1;
    }

    if (aacEncoder_SetParam(m_pHandle, AACENC_BITRATE, bitRate) != AACENC_OK) {
        LOGD("Unable to set the bitrate\n");
        return 1;
    }

    if (aacEncoder_SetParam(m_pHandle, AACENC_TRANSMUX, 0) != AACENC_OK) {
        LOGD("Unable to set the ADTS transmux\n");
        return 1;
    }
    if (aacEncoder_SetParam(m_pHandle, AACENC_AFTERBURNER, afterburner) != AACENC_OK) {
        LOGD("Unable to set the afterburner mode\n");
        return 1;
    }

    if (aacEncEncode(m_pHandle, NULL, NULL, NULL, NULL) != AACENC_OK) {
        LOGD("Unable to initialize the encoder\n");
        return 1;
    }

    AACENC_InfoStruct info = { 0 };
    int ret = 0;
    if ((ret=aacEncInfo(m_pHandle, &info)) != AACENC_OK) {
        LOGD("Unable to get the encoder info\n");
        return 1;
    }

    m_extraConfig = std::string((char*)info.confBuf, info.confSize);

    return ret;
}

std::string &FaacAudioEncoder::GetExtraConfig(MessageChain *chain) {
    return m_extraConfig;
}

void FaacAudioEncoder::CloseMedia(MediaPlugin *plugin) {
    aacEncClose(&m_pHandle);
}

int FaacAudioEncoder::ProcessMedia(MediaPlugin *plugin, Message msg) {
    AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
    AACENC_InArgs in_args = { 0 };
    AACENC_OutArgs out_args = { 0 };

    int in_identifier = IN_AUDIO_DATA;
    int in_size, in_elem_size;
    int out_identifier = OUT_BITSTREAM_DATA;
    int out_size, out_elem_size;
    void *in_ptr, *out_ptr;

    in_ptr = msg.data();
    in_size = m_inputSamples*2;
    in_elem_size = 2;

    in_args.numInSamples = m_inputSamples;
    in_buf.numBufs = 1;
    in_buf.bufs = &in_ptr;
    in_buf.bufferIdentifiers = &in_identifier;
    in_buf.bufSizes = &in_size;
    in_buf.bufElSizes = &in_elem_size;


    Message opkt = p_bufferPool->NewMessage(MsgKey_ProcessData, this);
    out_ptr = opkt.data();
    out_size = opkt.bufferSize();
    out_elem_size = 1;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes = &out_size;
    out_buf.bufElSizes = &out_elem_size;

    int ret;
    if ((ret = aacEncEncode(m_pHandle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
        LOGD("[FaacAudioEncoder] encode_frame failed %x\n", ret);
        return -11;
    }

//    LOGD("[FaacAudioEncoder] EncodeVideo out_size=%d  out_elem_size=%d\n", out_args.numOutBytes, out_elem_size);

    opkt.msgFrame()->type = MediaType_Audio;
    opkt.msgFrame()->size = out_args.numOutBytes;
    opkt.msgFrame()->data = opkt.data();
    opkt.msgFrame()->timestamp = msg.msgFrame()->timestamp / 1000;
    opkt.msgFrame()->flag = PktFlag_PFrame;

//    MessageChain::onMsgProcessData(opkt);
    onProcessedMedia(this, opkt);

    return 0;
}
