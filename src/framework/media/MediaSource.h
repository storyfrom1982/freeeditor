//
// Created by yongge on 20-3-15.
//

#ifndef ANDROID_MEDIASOURCE_H
#define ANDROID_MEDIASOURCE_H


#include "MessageChain.h"


namespace freee {


    class MediaSource : public MessageChain
    {
    public:
        static MediaSource* Create(std::string url);
        MediaSource(const std::string name);
        virtual ~MediaSource();

        json &GetConfig(MessageChain *chain) override;

        std::string &GetExtraConfig(MessageChain *chain) override;

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgStart(Message pkt) override;

        void onMsgStop(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

        void onMsgControl(Message pkt) override;


    protected:
        void onReadSource(Message msg);
        virtual int OpenSource(Message msg) = 0;
        virtual void CloseSource() = 0;
        virtual int ReadSource() = 0;

    private:
        void onMsgReadSource(Message msg);

    protected:
        bool m_isRunning = false;
        int m_streamCount = 0;
        int m_openedStream = 0;
        std::map<int, std::string> m_extraConfigMap;
        std::map<int, MessageChain*> m_streamMap;
    };

}


#endif //ANDROID_MEDIASOURCE_H
