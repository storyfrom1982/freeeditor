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

        void ConnectStream(std::string url) override;

        void DisconnectStream() override;

    protected:
        int OpenModule() override;

        void CloseModule() override;

        int ProcessMediaByModule(SmartPkt pkt) override;


    private:
        int m_streamCount;
        AVFormatContext *m_pContext;
        std::vector<AVStream*> m_Streams;
    };

}


#endif //ANDROID_FILEMEDIASTREAM_H
