//
// Created by yongge on 20-2-19.
//

#ifndef ANDROID_MEDIATREE_H
#define ANDROID_MEDIATREE_H


#include <MConfig.h>
#include <MessagePool.h>
#include "MessageProcessor.h"

namespace freee {

    enum {
        MediaType_All = 0,
        MediaType_Audio,
        MediaType_Video,
        MediaType_Text
    };

    enum {
        Status_Closed = 0,
        Status_Opened = 1,
        Status_Started = 2,
        Status_Stopped = 3,
    };

    class MessageChain : public MessageProcessor {

    public:
        MessageChain(std::string name, int type)
        : MessageProcessor(name), m_type(type){}

        virtual ~MessageChain(){
//            LOGD("[DELETE]<MediaChainImpl>[%s]\n", m_name.c_str());
            AutoLock lockOutput(m_lockOutputChain);
            m_outputChain.clear();
            AutoLock lockInput(m_lockInputChain);
            m_inputChain.clear();
            m_chainToStream.clear();
        }

        virtual void FinalClear() {};

        virtual void Open(MessageChain *chain) {
            ProcessMessage(Message(MsgKey_Open, chain));
        }

        virtual void Close(MessageChain *chain) {
            ProcessMessage(Message(MsgKey_Close, chain));
        }

        virtual void Start(MessageChain *chain) {
            ProcessMessage(Message(MsgKey_Start, chain));
        }

        virtual void Stop(MessageChain *chain) {
            ProcessMessage(Message(MsgKey_Stop, chain));
        }

        virtual void ProcessData(MessageChain *chain, Message msg) {
            msg.SetKey(MsgKey_ProcessData);
            msg.SetPtr(chain);
            ProcessMessage(msg);
        }

        virtual void ProcessEvent(MessageChain *chain, Message msg) {
            msg.SetKey(MsgKey_ProcessEvent);
            msg.SetPtr(chain);
            ProcessMessage(msg);
        }


    protected:
        void AddInput(MessageChain *chain) {
            if (chain){
                AutoLock lock(m_lockInputChain);
                m_inputChain.push_back(chain);
                m_chainToStream[chain] = nullptr;
            }
        }

        void DelInput(MessageChain *chain) {
            AutoLock lock(m_lockInputChain);
            auto it = std::find(m_inputChain.begin(), m_inputChain.end(), chain);
            if (it != m_inputChain.end()){
                m_inputChain.erase(it);
            }
            if (m_chainToStream.find(chain) != m_chainToStream.end()){
                m_chainToStream.erase(chain);
            }
        }

    public:
        void SetStreamId(int streamId){
            m_streamId = streamId;
        }

        virtual int GetType(MessageChain *chain) {
            return m_type;
        }

        virtual int GetStreamId(MessageChain *chain){
            return m_streamId;
        }

        virtual json &GetConfig(MessageChain *chain) {
            return m_config;
        }

        virtual std::string &GetExtraConfig(MessageChain *chain) {
            return m_extraConfig;
        }

        virtual void AddOutput(MessageChain *chain) {
            if (chain){
                AutoLock lock(m_lockOutputChain);
                this->m_outputChain.push_back(chain);
                chain->AddInput(this);
                if (m_status == Status_Opened){
                    chain->Open(this);
                }
            }
        }

        virtual void DelOutput(MessageChain *chain) {
            AutoLock lock(m_lockOutputChain);
            for (auto it = m_outputChain.cbegin(); it != m_outputChain.cend(); it++){
                if ((*it) == chain){
                    m_outputChain.erase(it);
                    chain->DelInput(this);
                    break;
                }
            }
        }

        void SetEventListener(MessageChain *listener) {
            AutoLock lock(m_lockEventListener);
            m_pEventListener = listener;
        }

        void SendEvent(Message pkt) {
            AutoLock lock(m_lockEventListener);
            if (m_pEventListener){
                m_pEventListener->ProcessEvent(this, pkt);
            }
        }


    protected:
        virtual void onMsgOpen(Message msg){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Open(this);
            }
        };
        virtual void onMsgClose(Message msg){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Close(this);
            }
        };
        virtual void onMsgStart(Message msg){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Start(this);
            }
        };
        virtual void onMsgStop(Message msg){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Stop(this);
            }
        };
        virtual void onMsgProcessData(Message msg){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->ProcessData(this, msg);
            }
        };
        virtual void onMsgProcessEvent(Message msg){
            AutoLock lock(m_lockInputChain);
            for (auto it = m_inputChain.cbegin(); it != m_inputChain.cend(); it++){
                if ((*it)){
                    (*it)->ProcessEvent(this, msg);
                }
            }
        };
        virtual void onMsgControl(Message msg){};

        //Async
        void MessageProcess(Message msg) override {
            switch (msg.GetKey()){
                case MsgKey_Open:
                    onMsgOpen(msg);
                    break;
                case MsgKey_Close:
                    onMsgClose(msg);
                    break;
                case MsgKey_Start:
                    onMsgStart(msg);
                    break;
                case MsgKey_Stop:
                    onMsgStop(msg);
                    break;
                case MsgKey_ProcessData:
                    onMsgProcessData(msg);
                    break;
                case MsgKey_ProcessEvent:
                    onMsgProcessEvent(msg);
                    break;
                case MsgKey_ProcessControl:
                default:
                    onMsgControl(msg);
            }
        }


    protected:
        virtual int OpenModule(){ return 0;};
        virtual void CloseModule(){};
        virtual int ProcessMediaByModule(Message pkt){ return 0;};
        virtual void onProcessMediaByModule(Message pkt){};


    protected:
        int m_status = Status_Closed;
        int m_outputChainStatus = Status_Closed;

        int m_type;
        int m_streamId;
        json m_config;
        std::string m_extraConfig;

        Lock m_lockEventListener;
        MessageChain *m_pEventListener = nullptr;

        Lock m_lockInputChain;
        std::map<void*, void*> m_chainToStream;
        std::vector<MessageChain*> m_inputChain;

        Lock m_lockOutputChain;
        std::vector<MessageChain*> m_outputChain;
    };

}


#endif //ANDROID_MEDIATREE_H
