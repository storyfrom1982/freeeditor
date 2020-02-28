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
            bufferPool = new BufferPool(2, 4096);
        }

        virtual ~MessageContext(){
            LOGD("~MessageContext()[%s]\n", name.c_str());
            delete bufferPool;
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

        SmartPkt GetJsonPkt(int key, std::string str){
            return bufferPool->GetPkt(key, str);
        }

    private:
        std::string name;
        BufferPool *bufferPool;
        Lock mLock;
        MessageContext *messageContext;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
