//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIACHAINIMPL_H
#define ANDROID_MEDIACHAINIMPL_H

#include <BufferPool.h>
#include "MediaChain.h"
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

    class MediaChainImpl : public MediaChain, public MessageProcessor {

    public:
        MediaChainImpl(int mediaType, int mediaNumber, std::string mediaName) :
            m_type(mediaType),
            m_number(mediaNumber),
            m_name(mediaName){}

        virtual ~MediaChainImpl(){
//            LOGD("[DELETE]<MediaChainImpl>[%s]\n", m_name.c_str());
            AutoLock lock(m_outputChainLock);
            m_outputChain.clear();
            m_inputChain.clear();
        }

        void Open(MediaChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgOpen, chain));
        }

        void Close(MediaChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgClose, chain));
        }

        void Start(MediaChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgStart, chain));
        }

        void Stop(MediaChain *chain) override {
            ProcessMessage(SmartPkt(PktMsgStop, chain));
        }

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override {
            pkt.SetKey(PktMsgProcessMedia);
            pkt.SetPtr(chain);
            ProcessMessage(pkt);
        }

        void onRecvEvent(MediaChain *chain, SmartPkt pkt) override {
            pkt.SetKey(PktMsgRecvEvent);
            pkt.SetPtr(chain);
            ProcessMessage(pkt);
        }

    protected:

        void SetStreamIndex(int id) override {
            m_streamIndex = id;
        }

        int GetStreamIndex() override {
            return m_streamIndex;
        }

        void AddPrev(MediaChain *chain) override {
            if (chain){
                AutoLock lock(m_inputChainLock);
                this->m_inputChain.push_back(chain);
            }
        }

        void DelPrev(MediaChain *chain) override {
            AutoLock lock(m_inputChainLock);
            for (int i = 0; i < m_inputChain.size(); ++i){
                if (m_inputChain[i] == chain){
                    m_inputChain.erase(m_inputChain.begin() + i);
                    break;
                }
            }
        }

    public:
        int GetType(MediaChain *chain) override {
            return m_type;
        }

        json &GetConfig(MediaChain *chain) override {
            return m_config;
        }

        int GetNumber(MediaChain *chain) override {
            return m_number;
        }

        std::string GetName(MediaChain *chain) override {
            return m_name;
        }

        virtual void AddNext(MediaChain *chain) override {
            if (chain){
                AutoLock lock(m_outputChainLock);
                this->m_outputChain.push_back(chain);
                chain->AddPrev(this);
            }
        }

        virtual void DelNext(MediaChain *chain) override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                if (m_outputChain[i] == chain){
                    m_outputChain.erase(m_outputChain.begin() + i);
                    chain->DelPrev(this);
                    break;
                }
            }
        }

//        virtual void SetEventCallback(MediaChain::EventCallback *callback) override {
//            AutoLock lock(m_callbackLock);
//            m_callbackList.push_back(callback);
////            p_callback = callback;
//        }
//
//    protected:
//        virtual void ReportEvent(SmartPkt pkt) {
//            AutoLock lock(m_callbackLock);
//            for (int i = 0; i < m_callbackList.size(); ++i){
//                m_callbackList[i]->onEvent(this, pkt);
//            }
////            if (p_callback){
////                p_callback->onEvent(this, pkt);
////            }
//        }


    protected:
        virtual void onMsgOpen(SmartPkt pkt){
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Open(this);
            }
        };
        virtual void onMsgClose(SmartPkt pkt){
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Close(this);
            }
        };
        virtual void onMsgStart(SmartPkt pkt){
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Start(this);
            }
        };
        virtual void onMsgStop(SmartPkt pkt){
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Stop(this);
            }
        };
        virtual void onMsgProcessMedia(SmartPkt pkt){
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->ProcessMedia(this, pkt);
            }
        };
        virtual void onMsgRecvEvent(SmartPkt pkt){
            AutoLock lock(m_inputChainLock);
            for (int i = 0; i < m_inputChain.size(); ++i){
                if (m_inputChain[i] != nullptr){
                    m_inputChain[i]->onRecvEvent(this, pkt);
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
                    onMsgProcessMedia(pkt);
                    break;
                case PktMsgRecvEvent:
                    onMsgRecvEvent(pkt);
                    break;
                case PktMsgControl:
                default:
                    onMsgControl(pkt);
            }
        }


    protected:
        int m_type;
        int m_number;
        int m_streamIndex;
        json m_config;
        std::string m_name;

//        Lock m_callbackLock;
////        MediaChain::EventCallback *p_callback = nullptr;
//        std::vector<EventCallback*> m_callbackList;

        Lock m_inputChainLock;
        std::vector<MediaChain*> m_inputChain;

        Lock m_outputChainLock;
        std::vector<MediaChain*> m_outputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
