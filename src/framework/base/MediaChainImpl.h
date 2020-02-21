//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIACHAINIMPL_H
#define ANDROID_MEDIACHAINIMPL_H

#include "MediaChain.h"
#include "AutoLock.h"


namespace freee {

    enum {
        Status_Closed = 0,
        Status_Opened = 1,
        Status_Started = 2,
        Status_Stopped = 3,
    };

    class MediaChainImpl : public MediaChain {

    public:
        MediaChainImpl(int mediaType, int mediaNumber, std::string mediaName) :
            mMediaType(mediaType),
            mMediaNumber(mediaNumber),
            mMediaName(mediaName){}

        virtual ~MediaChainImpl(){
            AutoLock lock(mOutputChainLock);
            mOutputChain.clear();
        }

        void Open(MediaChain *chain) override {}

        void Close(MediaChain *chain) override {}

        void Start(MediaChain *chain) override {}

        void Stop(MediaChain *chain) override {}

        void ProcessMedia(MediaChain *chain, MediaPacket pkt) override {}

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

        void Control(MediaChain *chain, MediaPacket pkt) override {}

        void SetCallback(MediaChain::Callback *callback) override {
            AutoLock lock(mCallbackLock);
            mCallback = callback;
        }

    protected:
        void ReportEvent(MediaPacket pkt) {
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
        int mMediaType;
        int mMediaNumber;

        json mMediaConfig;
        std::string mMediaName;

        Lock mCallbackLock;
        MediaChain::Callback *mCallback;

        Lock mOutputChainLock;
        std::vector<MediaChain*> mOutputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
