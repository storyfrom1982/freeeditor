//
// Created by yongge on 20-2-1.
//

#ifndef ANDROID_DEVICEINTERFACE_H
#define ANDROID_DEVICEINTERFACE_H


#include <string>

#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>
#include <video_resample.h>
#include <sr_buffer_pool.h>

#ifdef __cplusplus
}
#endif

#include <SrBufferPool.h>


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
            if (context){
                messageContext = context;
                messageContext->messageContext = this;
            }
        }

        void DisconnectContext(){
            if (messageContext){
                messageContext->messageContext = NULL;
                messageContext = NULL;
            }
        }

        void SetContextName(std::string name){
            this->name = name;
        }

    public:
        virtual void onReceiveMessage(SrPkt pkt) {

        };

        virtual SrPkt onObtainMessage(int key){
            return SrPkt();
        }

    protected:
        virtual void SendMessage(SrPkt pkt){
            if (messageContext){
                messageContext->onReceiveMessage(pkt);
            }
        }

        virtual SrPkt GetMessage(int key){
            if (messageContext){
                return messageContext->onObtainMessage(key);
            }
            return SrPkt();
        }

    private:
        std::string name;
        MessageContext *messageContext;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
