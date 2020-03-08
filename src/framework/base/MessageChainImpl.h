//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIACHAINIMPL_H
#define ANDROID_MEDIACHAINIMPL_H

#include <BufferPool.h>
#include "MessageChain.h"
#include "AutoLock.h"
#include "MessageProcessor.h"


namespace freee {

    enum {
        Status_Closed = 0,
        Status_Opened = 1,
        Status_Started = 2,
        Status_Stopped = 3,
    };

//    enum {
//        RecvMsg_Error = PktMsgError,
//        RecvMsg_Exit = PktMsgClosed,
//        PktMsgOpen = 1,
//        PktMsgStart = 2,
//        PktMsgStop = 3,
//        PktMsgClose = 4,
//        PktMsgProcessMedia = 5,
//        PktMsgControl = 6,
//    };

    class MessageChainImpl : public MessageChain {

    public:
        MessageChainImpl(int mediaType, int mediaNumber, std::string mediaName) :
            m_type(mediaType),
            m_number(mediaNumber),
            m_name(mediaName){}

        virtual ~MessageChainImpl(){
//            LOGD("[DELETE]<MediaChainImpl>[%s]\n", m_name.c_str());
            AutoLock lock(m_lockOutputChain);
            m_outputChain.clear();
            AutoLock autoLock(m_lockInputChain);
            m_inputChain.clear();
            m_chainToStream.clear();
        }

        void Open(MessageChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgOpen, chain));
        }

        void Close(MessageChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgClose, chain));
        }

        void Start(MessageChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgStart, chain));
        }

        void Stop(MessageChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgStop, chain));
        }

        void ProcessData(MessageChain *chain, SmartPkt pkt) override {
            pkt.SetKey(PktMsgProcessMedia);
            pkt.SetPtr(chain);
            ProcessMessage(pkt);
        }

        void ProcessEvent(MessageChain *chain, SmartPkt pkt) override {
            pkt.SetKey(PktMsgProcessEvent);
            pkt.SetPtr(chain);
            ProcessMessage(pkt);
        }


    protected:
        void AddInput(MessageChain *chain) override {
            if (chain){
                AutoLock lock(m_lockInputChain);
                m_inputChain.push_back(chain);
                m_chainToStream[chain] = nullptr;
            }
        }

        void DelInput(MessageChain *chain) override {
            AutoLock lock(m_lockInputChain);
            auto it = std::find(m_inputChain.begin(), m_inputChain.end(), chain);
            if (it != m_inputChain.end()){
                m_inputChain.erase(it);
            }
            if (m_chainToStream.find(chain) != m_chainToStream.end()){
                m_chainToStream.erase(chain);
            }
//            for (int i = 0; i < m_inputChain.size(); ++i){
//                if (m_inputChain[i] == chain){
//                    m_inputChain.erase(m_inputChain.begin() + i);
//                    if (m_chainToStream.find(chain) != m_chainToStream.end()){
//                        m_chainToStream.erase(chain);
//                    }
//                    break;
//                }
//            }
        }

    public:
        int GetType(MessageChain *chain) override {
            return m_type;
        }

        int GetNumber(MessageChain *chain) override {
            return m_number;
        }

        std::string GetName(MessageChain *chain) override {
            return m_name;
        }

        json &GetConfig(MessageChain *chain) override {
            return m_config;
        }

        std::string GetExtraConfig(MessageChain *chain) override {
            return m_extraConfig;
        }

        virtual void AddOutput(MessageChain *chain) override {
            if (chain){
                AutoLock lock(m_lockOutputChain);
                this->m_outputChain.push_back(chain);
                chain->AddInput(this);
            }
        }

        virtual void DelOutput(MessageChain *chain) override {
            AutoLock lock(m_lockOutputChain);
            for (auto it = m_outputChain.cbegin(); it != m_outputChain.cend(); it++){
                if ((*it) == chain){
                    m_outputChain.erase(it);
                    chain->DelInput(this);
                    break;
                }
            }
//            for (int i = 0; i < m_outputChain.size(); ++i){
//                if (m_outputChain[i] == chain){
//                    m_outputChain.erase(m_outputChain.begin() + i);
//                    chain->DelInput(this);
//                    break;
//                }
//            }
        }

        void SetEventListener(MessageChain *listener) override {
            AutoLock lock(m_lockEventListener);
            m_pEventListener = listener;
        }


    protected:
        virtual void SendEvent(SmartPkt pkt) {
            AutoLock lock(m_lockEventListener);
            if (m_pEventListener){
                m_pEventListener->ProcessEvent(this, pkt);
            }
        }


    protected:
        virtual void onMsgOpen(SmartPkt pkt){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Open(this);
            }
        };
        virtual void onMsgClose(SmartPkt pkt){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Close(this);
            }
        };
        virtual void onMsgStart(SmartPkt pkt){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Start(this);
            }
        };
        virtual void onMsgStop(SmartPkt pkt){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Stop(this);
            }
        };
        virtual void onMsgProcessData(SmartPkt pkt){
            AutoLock lock(m_lockOutputChain);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->ProcessData(this, pkt);
            }
        };
        virtual void onMsgProcessEvent(SmartPkt pkt){
            AutoLock lock(m_lockInputChain);
            for (auto it = m_inputChain.cbegin(); it != m_inputChain.cend(); it++){
                if ((*it)){
                    (*it)->ProcessEvent(this, pkt);
                }
            }
        };
        virtual void onMsgControl(SmartPkt pkt){};

        //Async
        void MessageProcess(SmartPkt pkt) override {
            switch (pkt.GetKey()){
                case PktMsgOpen:
                    onMsgOpen(pkt);
                    break;
                case PktMsgClose:
                    onMsgClose(pkt);
                    break;
                case PktMsgStart:
                    onMsgStart(pkt);
                    break;
                case PktMsgStop:
                    onMsgStop(pkt);
                    break;
                case PktMsgProcessMedia:
                    onMsgProcessData(pkt);
                    break;
                case PktMsgProcessEvent:
                    onMsgProcessEvent(pkt);
                    break;
                case PktMsgControl:
                default:
                    onMsgControl(pkt);
            }
        }


    protected:
        int m_type;
        int m_number;
        json m_config;
        std::string m_extraConfig;
        std::string m_name;

        Lock m_lockEventListener;
        MessageChain *m_pEventListener = nullptr;

        Lock m_lockInputChain;
        std::map<void*, void*> m_chainToStream;
        std::vector<MessageChain*> m_inputChain;

        Lock m_lockOutputChain;
        int m_outputChainStatus = 0;
        std::vector<MessageChain*> m_outputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
