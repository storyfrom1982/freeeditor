//
// Created by yongge on 20-5-13.
//

#ifndef ANDROID_MEDIACHAIN_H
#define ANDROID_MEDIACHAIN_H

#include "message/MessageChain.h"

namespace freee {


    class MediaPlugin : public MessageChain
    {
    public:
        MediaPlugin(const std::string &name);
        virtual ~MediaPlugin();

    protected:
        virtual int OpenMedia(MediaPlugin *plugin){ return -1;};
        virtual void CloseMedia(MediaPlugin *plugin){};
        virtual int ProcessMedia(MediaPlugin *plugin, Message msg){ return -1;};
        virtual void onProcessMedia(MediaPlugin *plugin, Message msg){
            MessageChain::onMsgProcessData(msg);
        };

    };

}



#endif //ANDROID_MEDIACHAIN_H
