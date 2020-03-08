//
// Created by yongge on 20-2-19.
//

#ifndef ANDROID_MEDIATREE_H
#define ANDROID_MEDIATREE_H


#include <MConfig.h>
#include <BufferPool.h>
#include "MessageProcessor.h"

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

    class MessageChain : public MessageProcessor {

    public:
        virtual ~MessageChain(){};
        virtual void FinalClear() = 0;

        virtual void Open(MessageChain *chain) = 0;
        virtual void Close(MessageChain *chain) = 0;
        virtual void Start(MessageChain *chain) = 0;
        virtual void Stop(MessageChain *chain) = 0;
        virtual void ProcessData(MessageChain *chain, SmartPkt pkt) = 0;
        virtual void ProcessEvent(MessageChain *chain, SmartPkt pkt) = 0;

        virtual void SetEventListener(MessageChain *listener) = 0;

        virtual int GetType(MessageChain *chain) = 0;
        virtual int GetNumber(MessageChain *chain) = 0;
        virtual json& GetConfig(MessageChain *chain) = 0;
        virtual std::string GetExtraConfig(MessageChain *chain) = 0;
        virtual std::string GetName(MessageChain *chain) = 0;

        virtual void AddInput(MessageChain *chain) = 0;
        virtual void DelInput(MessageChain *chain) = 0;

        virtual void AddOutput(MessageChain *chain) = 0;
        virtual void DelOutput(MessageChain *chain) = 0;
    };

}


#endif //ANDROID_MEDIATREE_H
