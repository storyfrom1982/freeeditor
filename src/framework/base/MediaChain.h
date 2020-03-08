//
// Created by yongge on 20-2-19.
//

#ifndef ANDROID_MEDIATREE_H
#define ANDROID_MEDIATREE_H


#include <MConfig.h>
#include <BufferPool.h>

namespace freee {

    enum {
        MediaType_All = 0,
        MediaType_Audio,
        MediaType_Video,
        MediaType_Text
    };

    enum {
        MediaNumber_Player = 0,
        MediaNumber_Recorder = 5000,

        MediaNumber_AudioRender = 10000,
        MediaNumber_VideoRender = 11000,
        MediaNumber_AudioDecoder = 12000,
        MediaNumber_VideoDecoder = 13000,
        MediaNumber_InputProtocal = 14000,

        MediaNumber_AudioSource = 50000,
        MediaNumber_VideoSource = 51000,
        MediaNumber_AudioEncoder = 52000,
        MediaNumber_VideoEncoder = 53000,
        MediaNumber_OutputProtocal = 54000,

        MediaNumber_AudioFilter = 100000,
        MediaNumber_VideoFilter = 500000,
    };

    class MediaChain {

    public:
        virtual ~MediaChain(){};
        virtual void FinalClear() = 0;

        virtual void Open(MediaChain *chain) = 0;
        virtual void Close(MediaChain *chain) = 0;
        virtual void Start(MediaChain *chain) = 0;
        virtual void Stop(MediaChain *chain) = 0;
        virtual void ProcessMedia(MediaChain *chain, SmartPkt pkt) = 0;
        virtual void onRecvEvent(MediaChain *chain, SmartPkt pkt) = 0;

        virtual int GetType(MediaChain *chain) = 0;
        virtual int GetNumber(MediaChain *chain) = 0;
        virtual json& GetConfig(MediaChain *chain) = 0;
        virtual std::string GetExtraConfig(MediaChain *chain) = 0;
        virtual std::string GetName(MediaChain *chain) = 0;

        virtual void AddInput(MediaChain *chain) = 0;
        virtual void DelInput(MediaChain *chain) = 0;

        virtual void AddOutput(MediaChain *chain) = 0;
        virtual void DelOutput(MediaChain *chain) = 0;

    protected:

//        class EventCallback {
//        public:
//            virtual void onOpened(MediaChain *chain) = 0;
//            virtual void onStarted(MediaChain *chain) = 0;
//            virtual void onStopped(MediaChain *chain) = 0;
//            virtual void onClosed(MediaChain *chain) = 0;
//            virtual void onEvent(MediaChain *chain, SmartPkt pkt) = 0;
//        };
//
//        virtual void SetEventCallback(EventCallback *callback) = 0;
    };

}


#endif //ANDROID_MEDIATREE_H
