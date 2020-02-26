//
// Created by yongge on 20-2-1.
//

#ifndef ANDROID_DEVICEINTERFACE_H
#define ANDROID_DEVICEINTERFACE_H


#include <string>
#include <BufferPool.h>
#include <AutoLock.h>


namespace freee{

    class MessageContext {

    public:

        MessageContext(){
            messageContext = NULL;
        }

        virtual ~MessageContext(){
            LOGD("~MessageContext()[%s]\n", name.c_str());
        };

        void ConnectContext(MessageContext *context){
            AutoLock lock(mLock);
            if (context){
                messageContext = context;
                AutoLock lockContext(messageContext->mLock);
                messageContext->messageContext = this;
            }
        }

        void DisconnectContext(){
            AutoLock lock(mLock);
            if (messageContext){
                AutoLock lockContext(messageContext->mLock);
                messageContext->messageContext = NULL;
                messageContext = NULL;
            }
        }

        void SetContextName(std::string name){
            this->name = name;
        }

    public:
        virtual void onRecvMessage(SmartPkt pkt) {};

        virtual SmartPkt onObtainMessage(int key){
            return SmartPkt();
        }

    protected:
        virtual void SendMessage(SmartPkt pkt){
            AutoLock lock(mLock);
            if (messageContext){
                messageContext->onRecvMessage(pkt);
            }
        }

        virtual SmartPkt GetMessage(int key){
            AutoLock lock(mLock);
            if (messageContext){
                return messageContext->onObtainMessage(key);
            }
            return SmartPkt();
        }

    private:
        std::string name;

        Lock mLock;
        MessageContext *messageContext;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
