//
// Created by yongge on 20-3-6.
//

#ifndef ANDROID_FILEMEDIASTREAM_H
#define ANDROID_FILEMEDIASTREAM_H


extern "C"{

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

}


#include "MediaStream.h"

namespace freee {


    class FileMediaStream : public MediaStream {

    public:
        FileMediaStream();
        ~FileMediaStream();

    protected:
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessData(SmartPkt pkt) override;

        void onMsgConnectStream(SmartPkt pkt) override;

        void onMsgDisconnectStream() override;

    private:
        AVStream* addAudioStream(MessageChain *chain);
        AVStream* addVideoStream(MessageChain *chain);

    private:
        int m_status = 0;
        AVFormatContext *m_pContext = nullptr;
        std::vector<AVStream*> m_Streams;
        std::map<MessageChain*, AVStream*> m_streamMap;
    };

}


#endif //ANDROID_FILEMEDIASTREAM_H
