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
        virtual int OpenModule(){ return 0;};
        virtual void CloseModule(){};
        virtual int ProcessMediaByModule(SmartPkt pkt){ return 0;};
        virtual void onProcessMediaByModule(SmartPkt pkt){};
    };

}



#endif //ANDROID_MEDIAMODULE_H
