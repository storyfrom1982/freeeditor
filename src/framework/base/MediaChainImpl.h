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

    enum {
        RecvMsg_Error = 0,
        RecvMsg_Open = 1,
        RecvMsg_Start = 2,
        RecvMsg_Stop = 3,
        RecvMsg_Close = 4,
        RecvMsg_ProcessMedia = 5,
        RecvMsg_Control = 6,
    };

    class MediaChainImpl : public MediaChain, public MessageProcessor {

    public:
        MediaChainImpl(int mediaType, int mediaNumber, std::string mediaName) :
            mType(mediaType),
            mNumber(mediaNumber),
            mName(mediaName){}

        virtual ~MediaChainImpl(){
            AutoLock lock(mOutputChainLock);
            mOutputChain.clear();
        }

        void Open(MediaChain *chain) override {
            ProcessMessage(SmartPkt(RecvMsg_Open, chain));
        }

        void Close(MediaChain *chain) override {
            ProcessMessage(SmartPkt(RecvMsg_Close, chain));
        }

        void Start(MediaChain *chain) override {
            ProcessMessage(SmartPkt(RecvMsg_Start, chain));
        }

        void Stop(MediaChain *chain) override {
            ProcessMessage(SmartPkt(RecvMsg_Stop, chain));
        }

        void ProcessMedia(MediaChain *chain, SmartPkt pkt) override {
            pkt.msg.key = RecvMsg_ProcessMedia;
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void onOpened() override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->Open(this);
            }
        }

        void onClosed() override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->Close(this);
            }
        }

        void onStarted() override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->Start(this);
            }
        }

        void onStopped() override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->Stop(this);
            }
        }

        void onProcessMedia(SmartPkt pkt) override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->ProcessMedia(this, pkt);
            }
        }

        int GetType(MediaChain *chain) override {
            return mType;
        }

        json &GetConfig(MediaChain *chain) override {
            return mConfig;
        }

        int GetNumber(MediaChain *chain) override {
            return mNumber;
        }

        std::string GetName(MediaChain *chain) override {
            return mName;
        }

        virtual void AddOutputChain(MediaChain *chain) override {
            if (chain){
                AutoLock lock(mOutputChainLock);
                this->mOutputChain.push_back(chain);
            }
        }

        virtual void RemoveOutputChain(MediaChain *chain) override {
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                if (mOutputChain[i] == chain){
                    mOutputChain.erase(mOutputChain.begin() + i);
                    break;
                }
            }
        }

        virtual void SetEventCallback(MediaChain::EventCallback *callback) override {
            AutoLock lock(mCallbackLock);
            mCallback = callback;
        }

    protected:
        virtual void ReportEvent(SmartPkt pkt) {
            AutoLock lock(mCallbackLock);
            if (mCallback){
                mCallback->onEvent(this, pkt);
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
            switch (pkt.msg.key){
                case RecvMsg_Open:
                    MessageOpen(pkt);
                    break;
                case RecvMsg_Close:
                    MessageClose(pkt);
                    break;
                case RecvMsg_Start:
                    MessageStart(pkt);
                    break;
                case RecvMsg_Stop:
                    MessageStop(pkt);
                    break;
                case RecvMsg_ProcessMedia:
                    MessageProcessMedia(pkt);
                    break;
                case RecvMsg_Control:
                default:
                    MessageControl(pkt);
            }
        }


    protected:
        int mType;
        int mNumber;
        json mConfig;
        std::string mName;

        Lock mCallbackLock;
        MediaChain::EventCallback *mCallback;

        Lock mOutputChainLock;
        std::vector<MediaChain*> mOutputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
