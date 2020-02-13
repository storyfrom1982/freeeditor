//
// Created by yongge on 19-8-12.
//

#ifndef ANDROID_MESSAGEPROCESSOR_H
#define ANDROID_MESSAGEPROCESSOR_H


#include <string>
#include <vector>

#include <AutoLock.h>
#include "DataFrame.h"


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
        MsgKey_ProcessData = 1,
        MsgKey_UpdateConfig = 2,
    };

    class DataStream {

    public:

        DataStream(DataStream *processor){
            statusProcessor = processor;
        };

        virtual ~DataStream(){
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
        }

        virtual void addInputStream(DataStream *processor){
            AutoLock lock(m_inputsLock);
            processor->m_outputs.push_back(this);
            this->m_inputs.push_back(processor);
        };

        virtual void removeInputStream(DataStream *processor){
            AutoLock lock(m_inputsLock);
            for (int i = 0; i < m_inputs.size(); ++i){
                if (m_inputs[i] == processor){
                    m_inputs.erase(m_inputs.begin() + i);
                    break;
                }
            }
        }

        virtual void addOutputStream(DataStream *processor){
            AutoLock lock(m_outputsLock);
            processor->m_inputs.push_back(this);
            this->m_outputs.push_back(processor);
        };

        virtual void removeOutputStream(DataStream *processor){
            AutoLock lock(m_outputsLock);
            for (int i = 0; i < m_outputs.size(); ++i){
                if (m_outputs[i] == processor){
                    m_outputs.erase(m_outputs.begin() + i);
                    break;
                }
            }
        }


        virtual void processData(data_frame_t *frame){

        }

        virtual void updateConfig(data_frame_t *frame){

        }


        virtual void msgToUpstream(DataStream *processor, sr_message_t msg){
            AutoLock lock(m_inputsLock);
            if (m_inputs.size() > 0){
                for (int i = 0; i < m_inputs.size(); ++i){
                    m_inputs[i]->msgFromDownstream(this, msg);
                }
            }
        };

        virtual void msgToDownstream(DataStream *processor, sr_message_t msg){
            AutoLock lock(m_outputsLock);
            if (m_outputs.size() > 0){
                for (int i = 0; i < m_outputs.size(); ++i){
                    m_outputs[i]->msgFromUpstream(this, msg);
                }
            }
        };


    protected:

        virtual void msgFromUpstream(DataStream *processor, sr_message_t msg){};

        virtual void msgFromDownstream(DataStream *processor, sr_message_t msg){};


    protected:

        Lock m_inputsLock;
        std::vector<DataStream*> m_inputs;

        Lock m_outputsLock;
        std::vector<DataStream*> m_outputs;

        Lock statusProcessorLock;
        DataStream *statusProcessor;

    };

}


#endif //ANDROID_MESSAGEPROCESSOR_H
