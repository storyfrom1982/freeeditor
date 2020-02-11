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


namespace freee{

    class MessageContext {

    public:

        MessageContext(){
            messageQueue = NULL;
            messageContext = NULL;
        }

        virtual ~MessageContext(){
            LOGD("~MessageContext()[%s]\n", contextName.c_str());
            if (messageQueue){
                sr_message_queue_release(&messageQueue);
            }
        };

        void SetContextName(std::string name){
            contextName = name;
        }

        virtual void ConnectContextHandler(MessageContext *contextHandler){
            if (contextHandler){
                messageContext = contextHandler;
                messageContext->messageContext = this;
            }
        }

    public:

        virtual void OnPutMessage(sr_message_t msg) {
            sr_message_queue_put(messageQueue, msg);
        };

        virtual void OnPutDataBuffer(sr_message_t msg){
            sr_message_queue_put(messageQueue, msg);
        }

        virtual sr_message_t OnGetMessage(sr_message_t msg){
            return __sr_null_msg;
        }

        virtual sr_message_t OnGetDataBuffer(){
            return __sr_null_msg;
        }

    protected:

        virtual void PutMessage(sr_message_t msg){
            if (messageContext){
                messageContext->OnPutMessage(msg);
            }
        }

        virtual sr_message_t GetMessage(sr_message_t msg){
            if (messageContext){
                return messageContext->OnGetMessage(msg);
            }
            return __sr_null_msg;
        }

    protected:

        virtual void MessageProcessor(sr_message_t msg){};

        void StartMessageProcessor(){
            messageQueue = sr_message_queue_create();
            messageProcessor.name = contextName.c_str();
            messageProcessor.handler = this;
            messageProcessor.process = MessageProcessorThread;
            sr_message_queue_start_processor(messageQueue, &messageProcessor);
        }

        void StopMessageProcessor(){
            sr_message_queue_release(&messageQueue);
        }


    private:

        static void MessageProcessorThread(sr_message_processor_t *processor, sr_message_t msg){
            static_cast<MessageContext *>(processor->handler)->MessageProcessor(msg);
        }


    private:

        std::string contextName;

        MessageContext *messageContext;

        sr_message_queue_t *messageQueue;
        sr_message_processor_t messageProcessor;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
