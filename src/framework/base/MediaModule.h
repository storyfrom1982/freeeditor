//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIAMODULE_H
#define ANDROID_MEDIAMODULE_H


#include "MediaChainImpl.h"

namespace freee {


    class MediaModule : public MediaChainImpl{

    protected:
        virtual int OpenModule(json &cfg) = 0;
        virtual void CloseModule() = 0;
        virtual int StartModule() = 0;
        virtual void StopModule() = 0;
        virtual int ModuleProcessMedia(MediaPacket pkt) = 0;

        virtual void onModuleEvent(MediaPacket pkt) = 0;
        virtual void onModuleError(MediaPacket pkt) = 0;
        virtual void onModuleProcessMedia(MediaPacket pkt) = 0;


    protected:
        bool isOpened;
        bool isStarted;
    };

}



#endif //ANDROID_MEDIAMODULE_H
