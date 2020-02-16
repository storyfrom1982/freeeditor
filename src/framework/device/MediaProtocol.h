//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_MEDIAPROTOCOL_H
#define ANDROID_MEDIAPROTOCOL_H


#include <MessageContext.h>


namespace freee {


    class MediaProtocol : public MessageContext {

    public:

        static MediaProtocol* Create(std::string url);

        MediaProtocol(std::string url);
        ~MediaProtocol();

        void OnPutMessage(sr_message_t msg) override;

    protected:

        void ProcessMessage(sr_message_t msg) override;

        virtual void Connect(std::string url) = 0;

        virtual void Disconnect() = 0;

        virtual void Write(sr_message_t msg) = 0;

        virtual void Read(sr_message_t msg) = 0;
    };


}


#endif //ANDROID_MEDIAPROTOCOL_H
