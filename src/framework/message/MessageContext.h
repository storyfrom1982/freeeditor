//
// Created by yongge on 20-2-1.
//

#ifndef ANDROID_DEVICEINTERFACE_H
#define ANDROID_DEVICEINTERFACE_H


#include <AutoLock.h>
#include "MessagePool.h"


namespace freee{

    class MessageContext : public MessagePool{

    public:

        MessageContext(std::string name) : MessagePool(name)
        {
            m_name = name;
        }

        virtual ~MessageContext(){};

        virtual void ConnectContext(MessageContext *pMessageContext)
        {
            AutoLock lock(m_lock);
            if (pMessageContext)
            {
                p_messageContext = pMessageContext;
                AutoLock lockContext(p_messageContext->m_lock);
                p_messageContext->p_messageContext = this;
            }
        }

        void DisconnectContext()
        {
            AutoLock lock(m_lock);
            if (p_messageContext)
            {
                AutoLock lockContext(p_messageContext->m_lock);
                p_messageContext->p_messageContext = NULL;
                p_messageContext = NULL;
            }
        }

        std::string GetName()
        {
            return m_name;
        }

    protected:
        virtual void onRecvMessage(Message msg) {};

        virtual Message onRequestMessage(int key)
        {
            return Message();
        }

        virtual void SendMessage(Message msg)
        {
            AutoLock lock(m_lock);
            if (p_messageContext)
            {
                p_messageContext->onRecvMessage(msg);
            }
        }

        virtual Message RequestMessage(int key)
        {
            AutoLock lock(m_lock);
            if (p_messageContext)
            {
                return p_messageContext->onRequestMessage(key);
            }
            return Message();
        }

    private:
        Lock m_lock;
        std::string m_name;
        MessageContext *p_messageContext = nullptr;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
