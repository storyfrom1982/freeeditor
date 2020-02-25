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
        virtual int ModuleOpen(json &cfg){ return 0;};
        virtual void ModuleClose(){};
        virtual int ModuleProcessMedia(SmartPkt pkt){ return 0;};
        virtual void onModuleProcessMedia(SmartPkt pkt){};
    };

}



#endif //ANDROID_MEDIAMODULE_H
