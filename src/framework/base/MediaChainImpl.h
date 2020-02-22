//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIACHAINIMPL_H
#define ANDROID_MEDIACHAINIMPL_H

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
            mMediaType(mediaType),
            mMediaNumber(mediaNumber),
            mMediaName(mediaName){}

        virtual ~MediaChainImpl(){
            AutoLock lock(mOutputChainLock);
            mOutputChain.clear();
        }

        void Open(MediaChain *chain) override {
            MediaPacket pkt(RecvMsg_Open);
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void Close(MediaChain *chain) override {
            MediaPacket pkt(RecvMsg_Close);
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void Start(MediaChain *chain) override {
            MediaPacket pkt(RecvMsg_Start);
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void Stop(MediaChain *chain) override {
            MediaPacket pkt(RecvMsg_Stop);
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void ProcessMedia(MediaChain *chain, MediaPacket pkt) override {
            pkt.msg.key = RecvMsg_ProcessMedia;
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        void Control(MediaChain *chain, MediaPacket pkt) override {
            pkt.msg.key = RecvMsg_Control;
            pkt.msg.ptr = chain;
            ProcessMessage(pkt);
        }

        int GetMediaType(MediaChain *chain) override {
            return mMediaType;
        }

        json &GetMediaConfig(MediaChain *chain) override {
            return mMediaConfig;
        }

        int GetMediaNumber(MediaChain *chain) override {
            return mMediaNumber;
        }

        std::string GetMediaName(MediaChain *chain) override {
            return mMediaName;
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
        virtual void SendEvent(MediaPacket pkt) {
            AutoLock lock(mCallbackLock);
            if (mCallback){
                mCallback->onEvent(this, pkt);
            }
        }

        virtual void OutputMediaPacket(MediaPacket pkt){
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->ProcessMedia(this, pkt);
            }
        }


    protected:
        virtual void MessageOpen(MediaPacket pkt){};
        virtual void MessageClose(MediaPacket pkt){};
        virtual void MessageStart(MediaPacket pkt){};
        virtual void MessageStop(MediaPacket pkt){};
        virtual void MessagePacket(MediaPacket pkt){};
        virtual void MessageControl(MediaPacket pkt){};

        void MessageProcess(MediaPacket pkt) override {
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
                    MessagePacket(pkt);
                    break;
                case RecvMsg_Control:
                default:
                    MessageControl(pkt);
            }
        }


    protected:
        int mMediaType;

        int mMediaNumber;

        json mMediaConfig;
        std::string mMediaName;

        Lock mCallbackLock;
        MediaChain::EventCallback *mCallback;

        Lock mOutputChainLock;
        std::vector<MediaChain*> mOutputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
