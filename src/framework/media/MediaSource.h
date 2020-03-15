//
// Created by yongge on 20-3-15.
//

#ifndef ANDROID_MEDIASOURCE_H
#define ANDROID_MEDIASOURCE_H


#include <MessageChain.h>

namespace freee {


    class MediaSource : public MessageChain
    {
    public:
        MediaSource(const std::string &name = "MediaSource", int type = MediaType_All);

    };

}


#endif //ANDROID_MEDIASOURCE_H
