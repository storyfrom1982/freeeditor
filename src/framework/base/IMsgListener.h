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


    class IMsgListener {

    public:

        IMsgListener(){};

        virtual ~IMsgListener(){
            {
                AutoLock lockInputs(m_inputsLock);
                for (int i = 0; i < m_inputs.size(); ++i){
                    m_inputs[i]->removeOutput(this);
                }
                m_inputs.clear();
            }
            {
                AutoLock lockOutputs(m_outputsLock);
                for (int i = 0; i < m_outputs.size(); ++i){
                    m_outputs[i]->removeInput(this);
                }
                m_outputs.clear();
            }
        };

        virtual void addInput(IMsgListener *input){
            AutoLock lock(m_inputsLock);
            input->m_outputs.push_back(this);
            this->m_inputs.push_back(input);
        };

        virtual void addOutput(IMsgListener *receiver){
            AutoLock lock(m_outputsLock);
            receiver->m_inputs.push_back(this);
            this->m_outputs.push_back(receiver);
        };

        virtual void removeInput(IMsgListener *listener){
            AutoLock lock(m_inputsLock);
            for (int i = 0; i < m_inputs.size(); ++i){
                if (m_inputs[i] == listener){
                    m_inputs.erase(m_inputs.begin() + i);
                    break;
                }
            }
        }

        virtual void removeOutput(IMsgListener *listener){
            AutoLock lock(m_outputsLock);
            for (int i = 0; i < m_outputs.size(); ++i){
                if (m_outputs[i] == listener){
                    m_outputs.erase(m_outputs.begin() + i);
                    break;
                }
            }
        }

        virtual void onMsgFromOutput(sr_msg_t msg){};

        virtual void sendMsgToInput(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.size() > 0){
                for (int i = 0; i < m_inputs.size(); ++i){
                    m_inputs[i]->onMsgFromOutput(msg);
                }
            }
        };

        virtual void onMsgFromInput(sr_msg_t msg){};

        virtual void sendMsgToOutput(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.size() > 0){
                for (int i = 0; i < m_outputs.size(); ++i){
                    m_outputs[i]->onMsgFromInput(msg);
                }
            }
        };

        virtual sr_msg_t onInputRequest(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.empty()){
                return __sr_bad_msg;
            }
            return m_outputs.front()->onInputRequest(msg);
        };

        virtual sr_msg_t requestToOutput(sr_msg_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.empty()){
                return __sr_bad_msg;
            }
            return m_outputs.front()->onInputRequest(msg);
        }

        virtual sr_msg_t onOutputRequest(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.empty()){
                return __sr_bad_msg;
            }
            return m_inputs.front()->onOutputRequest(msg);
        };

        virtual sr_msg_t requestToInput(sr_msg_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.empty()){
                return __sr_bad_msg;
            }
            return m_inputs.front()->onOutputRequest(msg);
        }

    protected:

        StaticMutex m_inputsLock;
        StaticMutex m_outputsLock;
        std::vector<IMsgListener*> m_inputs;
        std::vector<IMsgListener*> m_outputs;
    };


}


#endif //ANDROID_IMSGLISTENER_H
