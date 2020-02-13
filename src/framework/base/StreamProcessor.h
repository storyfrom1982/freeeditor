//
// Created by yongge on 19-5-31.
//

#ifndef ANDROID_IMSGLISTENER_H
#define ANDROID_IMSGLISTENER_H


#include <string>
#include <vector>

#include <AutoLock.h>


#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>

#ifdef __cplusplus
}
#endif


namespace freee {

    enum {
//        MsgKey_Null = -1,
//        MsgKey_OK = 0,
        MsgKey_ProcessData = 1,
        MsgKey_UpdateConfig = 2
    };

    class StreamProcessor {

    public:

        StreamProcessor(){};

        virtual ~StreamProcessor(){
            {
                AutoLock lockInputs(m_inputsLock);
                for (int i = 0; i < m_inputs.size(); ++i){
                    if (m_inputs[i] != NULL){
                        m_inputs[i]->removeOutputStream(this);
                    }
                }
                m_inputs.clear();
            }
            {
                AutoLock lockOutputs(m_outputsLock);
                for (int i = 0; i < m_outputs.size(); ++i){
                    if (m_outputs[i] != NULL){
                        m_outputs[i]->removeInputStream(this);
                    }
                }
                m_outputs.clear();
            }
        };

        virtual void addInputStream(StreamProcessor *stream){
            AutoLock lock(m_inputsLock);
            stream->m_outputs.push_back(this);
            this->m_inputs.push_back(stream);
        };

        virtual void removeInputStream(StreamProcessor *stream){
            AutoLock lock(m_inputsLock);
            for (int i = 0; i < m_inputs.size(); ++i){
                if (m_inputs[i] == stream){
                    m_inputs.erase(m_inputs.begin() + i);
                    break;
                }
            }
        }

        virtual void addOutputStream(StreamProcessor *stream){
            AutoLock lock(m_outputsLock);
            stream->m_inputs.push_back(this);
            this->m_outputs.push_back(stream);
        };

        virtual void removeOutputStream(StreamProcessor *stream){
            AutoLock lock(m_outputsLock);
            for (int i = 0; i < m_outputs.size(); ++i){
                if (m_outputs[i] == stream){
                    m_outputs.erase(m_outputs.begin() + i);
                    break;
                }
            }
        }

        virtual void sendMessageToInputStream(sr_message_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.size() > 0){
                for (int i = 0; i < m_inputs.size(); ++i){
                    if (m_inputs[i] != NULL){
                        m_inputs[i]->messageFromOutputStream(msg);
                    }
                }
            }else {
                messageFromOutputStream(msg);
            }
        };

        virtual void sendMessageToOutputStream(sr_message_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.size() > 0){
                for (int i = 0; i < m_outputs.size(); ++i){
                    if (m_outputs[i] != NULL){
                        m_outputs[i]->messageFromInputStream(msg);
                    }
                }
            }else {
                messageFromInputStream(msg);
            }
        };

        virtual sr_message_t sendRequestToInputStream(sr_message_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.empty()){
                return requestFromOutputStream(msg);
            }
            return m_inputs.front()->requestFromOutputStream(msg);
        }

        virtual sr_message_t sendRequestToOutputStream(sr_message_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.empty()){
                return requestFromInputStream(msg);
            }
            return m_outputs.front()->requestFromInputStream(msg);
        }

        virtual sr_message_t sendRequestToInputStream(sr_message_t msg, unsigned int id){
            AutoLock lock(m_inputsLock);
            if (id < m_inputs.size() && m_inputs[id] != NULL){
                return m_inputs[id]->requestFromOutputStream(msg);
            }
            return requestFromOutputStream(msg);
        }

        virtual sr_message_t sendRequestToOutputStream(sr_message_t msg, unsigned int id){
            AutoLock lock(m_outputsLock);
            if (id < m_outputs.size() && m_outputs[id] != NULL){
                return m_outputs[id]->requestFromInputStream(msg);
            }
            return requestFromInputStream(msg);
        }


    protected:

        virtual void messageFromInputStream(sr_message_t msg){

        }

        virtual void messageFromOutputStream(sr_message_t msg){

        }

        virtual sr_message_t requestFromInputStream(sr_message_t msg){
            return __sr_null_msg;
        }

        virtual sr_message_t requestFromOutputStream(sr_message_t msg){
            return __sr_null_msg;
        }


    protected:

        Lock m_inputsLock;
        Lock m_outputsLock;

        std::vector<StreamProcessor*> m_inputs;
        std::vector<StreamProcessor*> m_outputs;
    };


}


#endif //ANDROID_IMSGLISTENER_H
