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

        void onMsgProcessMedia(SmartPkt pkt) override;

        void onMsgConnectStream(SmartPkt pkt) override;

        void onMsgDisconnectStream() override;

    private:
        AVStream* addAudioStream(MediaChain *chain);
        AVStream* addVideoStream(MediaChain *chain);

    private:
        int m_streamCount;
        AVFormatContext *m_pContext;
        std::vector<AVStream*> m_Streams;
        std::map<MediaChain*, AVStream*> m_streamMap;
    };

}


#endif //ANDROID_FILEMEDIASTREAM_H
