//
// Created by yongge on 20-2-1.
//

#ifndef ANDROID_DEVICEINTERFACE_H
#define ANDROID_DEVICEINTERFACE_H


#include <string>
#include <MediaBufferPool.h>
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
                messageContext->messageContext = this;
            }
        }

        void DisconnectContext(){
            AutoLock lock(mLock);
            if (messageContext){
                messageContext->messageContext = NULL;
                messageContext = NULL;
            }
        }

        void SetContextName(std::string name){
            this->name = name;
        }

    public:
        virtual void onReceiveMessage(MediaPacket pkt) {

        };

        virtual MediaPacket onObtainMessage(int key){
            return MediaPacket();
        }

    protected:
        virtual void SendMessage(MediaPacket pkt){
            AutoLock lock(mLock);
            if (messageContext){
                messageContext->onReceiveMessage(pkt);
            }
        }

        virtual MediaPacket GetMessage(int key){
            AutoLock lock(mLock);
            if (messageContext){
                return messageContext->onObtainMessage(key);
            }
            return MediaPacket();
        }

    private:
        std::string name;

        Lock mLock;
        MessageContext *messageContext;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
