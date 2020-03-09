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
            p_messageContext = NULL;
            p_bufferPool = new BufferPool(2, 4096);
        }

        virtual ~MessageContext(){
//            LOGD("[DELETE]<MessageContext>[%s]\n", m_name.c_str());
            delete p_bufferPool;
        };

        virtual void ConnectContext(MessageContext *context){
            AutoLock lock(m_lock);
            if (context){
                p_messageContext = context;
                AutoLock lockContext(p_messageContext->m_lock);
                p_messageContext->p_messageContext = this;
            }
        }

        void DisconnectContext(){
            AutoLock lock(m_lock);
            if (p_messageContext){
                AutoLock lockContext(p_messageContext->m_lock);
                p_messageContext->p_messageContext = NULL;
                p_messageContext = NULL;
            }
        }

        void SetContextName(std::string name){
            m_name = name;
            p_bufferPool->SetName(m_name);
        }

    public:
        virtual void onRecvMessage(Message pkt) {};

        virtual Message onObtainMessage(int key){
            return Message();
        }

    protected:
        virtual void SendMessage(Message pkt){
            AutoLock lock(m_lock);
            if (p_messageContext){
                p_messageContext->onRecvMessage(pkt);
            }
        }

        virtual Message RequestMessage(int key){
            AutoLock lock(m_lock);
            if (p_messageContext){
                return p_messageContext->onObtainMessage(key);
            }
            return Message();
        }

        Message NewJsonPkt(int key, std::string str){
            return p_bufferPool->NewStringMessage(key, str);
        }

    private:
        Lock m_lock;
        std::string m_name;
        BufferPool *p_bufferPool;
        MessageContext *p_messageContext;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
