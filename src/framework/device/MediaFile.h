//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_MEDIAFILE_H
#define ANDROID_MEDIAFILE_H


#include "MediaProtocol.h"


namespace freee {

    class MediaFile : public MediaProtocol{
    public:
        MediaFile(const std::string &url);


    protected:
        void Connect(std::string url) override;

        void Disconnect() override;

        void Write(MediaPacket msg) override;

        void Read(MediaPacket msg) override;
    };

}


#endif //ANDROID_MEDIAFILE_H
