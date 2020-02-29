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
//        RecvMsg_Exit = PktMsgExit,
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
//            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void onOpened() override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Open(this);
            }
        }

        void onClosed() override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Close(this);
            }
        }

        void onStarted() override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Start(this);
            }
        }

        void onStopped() override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->Stop(this);
            }
        }

        void onProcessMedia(SmartPkt pkt) override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                m_outputChain[i]->ProcessMedia(this, pkt);
            }
        }

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

        virtual void AddOutputChain(MediaChain *chain) override {
            if (chain){
                AutoLock lock(m_outputChainLock);
                this->m_outputChain.push_back(chain);
            }
        }

        virtual void RemoveOutputChain(MediaChain *chain) override {
            AutoLock lock(m_outputChainLock);
            for (int i = 0; i < m_outputChain.size(); ++i){
                if (m_outputChain[i] == chain){
                    m_outputChain.erase(m_outputChain.begin() + i);
                    break;
                }
            }
        }

        virtual void SetEventCallback(MediaChain::EventCallback *callback) override {
            AutoLock lock(m_callbackLock);
            p_callback = callback;
        }

    protected:
        virtual void ReportEvent(SmartPkt pkt) {
            AutoLock lock(m_callbackLock);
            if (p_callback){
                p_callback->onEvent(this, pkt);
            }
        }


    protected:
        virtual void MessageOpen(SmartPkt pkt){};
        virtual void MessageClose(SmartPkt pkt){};
        virtual void MessageStart(SmartPkt pkt){};
        virtual void MessageStop(SmartPkt pkt){};
        virtual void MessageProcessMedia(SmartPkt pkt){};
        virtual void MessageControl(SmartPkt pkt){};

        void MessageProcess(SmartPkt pkt) override {
            switch (pkt.GetKey()){
                case PktMsgOpen:
                    MessageOpen(pkt);
                    break;
                case PktMsgClose:
                    MessageClose(pkt);
                    break;
                case PktMsgStart:
                    MessageStart(pkt);
                    break;
                case PktMsgStop:
                    MessageStop(pkt);
                    break;
                case PktMsgProcessMedia:
                    MessageProcessMedia(pkt);
                    break;
                case PktMsgControl:
                default:
                    MessageControl(pkt);
            }
        }


    protected:
        int m_type;
        int m_number;
        json m_config;
        std::string m_name;

        Lock m_callbackLock;
        MediaChain::EventCallback *p_callback;

        Lock m_outputChainLock;
        std::vector<MediaChain*> m_outputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
