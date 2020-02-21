//
// Created by yongge on 20-2-20.
//

#ifndef ANDROID_MEDIACHAINIMPL_H
#define ANDROID_MEDIACHAINIMPL_H

#include "MediaChain.h"
#include "AutoLock.h"


namespace freee {


    class MediaChainImpl : public MediaChain {

    public:
        virtual ~MediaChainImpl(){
            AutoLock lock(mOutputChainLock);
            mOutputChain.clear();
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

        void SetCallback(MediaChain::Callback *callback){
            AutoLock lock(mCallbackLock);
            mCallback = callback;
        }

        void Control(MediaChain *chain, MediaPacket pkt) override {}

    protected:
        void onEvent(MediaPacket pkt) {
            AutoLock lock(mCallbackLock);
            if (mCallback){
                mCallback->onEvent(this, pkt);
            }
        }

        void onError(MediaPacket pkt) {
            AutoLock lock(mCallbackLock);
            if (mCallback){
                mCallback->onError(this, pkt);
            }
        }

        void onProcessMedia(MediaPacket pkt) {
            AutoLock lock(mCallbackLock);
            if (mCallback){
                mCallback->onProcessMedia(this, pkt);
            }
        }

        void OutputMediaPacket(MediaPacket pkt){
            AutoLock lock(mOutputChainLock);
            for (int i = 0; i < mOutputChain.size(); ++i){
                mOutputChain[i]->ProcessMedia(this, pkt);
            }
        }


    protected:
        Lock mCallbackLock;
        MediaChain::Callback *mCallback;

        Lock mOutputChainLock;
        std::vector<MediaChain*> mOutputChain;

    };

}


#endif //ANDROID_MEDIACHAINIMPL_H
