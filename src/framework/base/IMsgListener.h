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
# include <AVFramePool.h>

#ifdef __cplusplus
}
#endif


namespace freee {


    class IMsgListener {

    public:

        IMsgListener(){};

        virtual ~IMsgListener(){
            {
                AutoLock lockInputs(m_inputsLock);
                for (int i = 0; i < m_inputs.size(); ++i){
                    m_inputs[i]->removeOutputStream(this);
                }
                m_inputs.clear();
            }
            {
                AutoLock lockOutputs(m_outputsLock);
                for (int i = 0; i < m_outputs.size(); ++i){
                    m_outputs[i]->removeInputStream(this);
                }
                m_outputs.clear();
            }
        };

        virtual void addInputStream(IMsgListener *input){
            AutoLock lock(m_inputsLock);
            input->m_outputs.push_back(this);
            this->m_inputs.push_back(input);
        };

        virtual void removeInputStream(IMsgListener *listener){
            AutoLock lock(m_inputsLock);
            for (int i = 0; i < m_inputs.size(); ++i){
                if (m_inputs[i] == listener){
                    m_inputs.erase(m_inputs.begin() + i);
                    break;
                }
            }
        }

        virtual void addOutputStream(IMsgListener *receiver){
            AutoLock lock(m_outputsLock);
            receiver->m_inputs.push_back(this);
            this->m_outputs.push_back(receiver);
        };

        virtual void removeOutputStream(IMsgListener *listener){
            AutoLock lock(m_outputsLock);
            for (int i = 0; i < m_outputs.size(); ++i){
                if (m_outputs[i] == listener){
                    m_outputs.erase(m_outputs.begin() + i);
                    break;
                }
            }
        }

        virtual void onMessageFromDownstream(sr_msg_t msg){};

        virtual void sendMessageToUpstream(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.size() > 0){
                for (int i = 0; i < m_inputs.size(); ++i){
                    m_inputs[i]->onMessageFromDownstream(msg);
                }
            }
        };

        virtual void onMessageFromUpstream(sr_msg_t msg){};

        virtual void sendMessageToDownstream(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.size() > 0){
                for (int i = 0; i < m_outputs.size(); ++i){
                    m_outputs[i]->onMessageFromUpstream(msg);
                }
            }
        };

        virtual sr_msg_t onRequestFromUpstream(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.empty()){
                return __sr_bad_msg;
            }
            return m_outputs.front()->onRequestFromUpstream(msg);
        };

        virtual sr_msg_t sendRequestToDownstream(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.empty()){
                return __sr_bad_msg;
            }
            return m_outputs.front()->onRequestFromUpstream(msg);
        }

        virtual sr_msg_t onRequestFromDownstream(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.empty()){
                return __sr_bad_msg;
            }
            return m_inputs.front()->onRequestFromDownstream(msg);
        };

        virtual sr_msg_t sendRequestToUpstream(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.empty()){
                return __sr_bad_msg;
            }
            return m_inputs.front()->onRequestFromDownstream(msg);
        }

    protected:

        Mutex m_inputsLock;
        Mutex m_outputsLock;

        std::vector<IMsgListener*> m_inputs;
        std::vector<IMsgListener*> m_outputs;
    };


}


#endif //ANDROID_IMSGLISTENER_H
