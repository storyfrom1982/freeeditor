//
// Created by yongge on 20-3-5.
//

#ifndef ANDROID_MEDIASTREAM_H
#define ANDROID_MEDIASTREAM_H


#include "../message/MessageChain.h"

namespace freee {

    class MediaStream : public MessageChain {
    public:
        static MediaStream* Create(std::string name);

        MediaStream(const std::string name);
        virtual ~MediaStream();

        void FinalClear() override;

        virtual void ConnectStream(std::string url);

        virtual void DisconnectStream();

    protected:

        void onMsgControl(Message pkt) override;

        virtual void onMsgConnectStream(Message pkt);

        virtual void onMsgDisconnectStream();

    protected:
        std::vector<json> m_configList;
    };

}


#endif //ANDROID_MEDIASTREAM_H
