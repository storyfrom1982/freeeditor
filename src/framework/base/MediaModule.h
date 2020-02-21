//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAMODULE_H
#define ANDROID_MEDIAMODULE_H


#include "MediaChainImpl.h"

namespace freee {


    class MediaModule : public MediaChainImpl{

    public:
        MediaModule(int mediaType, int mediaNumber, const std::string &mediaName)
                : MediaChainImpl(mediaType, mediaNumber, mediaName){};

        virtual ~MediaModule(){};

    protected:
        virtual void ProcessOpen(){};
        virtual void ProcessClose(){};
        virtual void ProcessStart(){};
        virtual void ProcessStop(){};
        virtual void ProcessPacket(MediaPacket pkt){};

        virtual int ModuleOpen(json &cfg){ return 0;};
        virtual void ModuleClose(){};
        virtual int ModuleProcessPacket(MediaPacket pkt){ return 0;};

        void OutputMediaPacket(MediaPacket pkt) override {
            MediaChainImpl::OutputMediaPacket(pkt);
        }
    };

}



#endif //ANDROID_MEDIAMODULE_H
