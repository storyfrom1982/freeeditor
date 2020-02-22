//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_MEDIAPROTOCOL_H
#define ANDROID_MEDIAPROTOCOL_H


#include <MessageContext.h>
#include <SrMessageQueue.h>


namespace freee {


    class MediaProtocol : public MessageContext, SrMessageQueue {

    public:

        static MediaProtocol* Create(std::string url);

        MediaProtocol(std::string url);
        ~MediaProtocol();

        void onRecvMessage(SmartPkt msg) override;

    protected:
    private:
        void MessageProcessor(SmartPkt pkt) override;

    protected:

        virtual void Connect(std::string url) = 0;

        virtual void Disconnect() = 0;

        virtual void Write(SmartPkt msg) = 0;

        virtual void Read(SmartPkt msg) = 0;
    };


}


#endif //ANDROID_MEDIAPROTOCOL_H
