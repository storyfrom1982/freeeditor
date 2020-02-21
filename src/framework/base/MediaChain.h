//
// Created by yongge on 20-2-19.
//

#ifndef ANDROID_MEDIATREE_H
#define ANDROID_MEDIATREE_H


#include <MConfig.h>
#include <MediaBufferPool.h>

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
        virtual void Open(MediaChain *chain) = 0;
        virtual void Close(MediaChain *chain) = 0;
        virtual void Start(MediaChain *chain) = 0;
        virtual void Stop(MediaChain *chain) = 0;
        virtual void Control(MediaChain *chain, MediaPacket pkt) = 0;
        virtual void ProcessMedia(MediaChain *chain, MediaPacket pkt) = 0;

        virtual int GetMediaType(MediaChain *chain) = 0;
        virtual int GetMediaNumber(MediaChain *chain) = 0;
        virtual json& GetMediaConfig(MediaChain *chain) = 0;
        virtual std::string GetMediaName(MediaChain *chain) = 0;

        virtual void AddOutputChain(MediaChain *chain) = 0;
        virtual void RemoveOutputChain(MediaChain *chain) = 0;

        class Callback {
        public:
            virtual void onEvent(MediaChain *chain, MediaPacket pkt) = 0;
        };

        virtual void SetCallback(Callback *callback) = 0;
    };

}


#endif //ANDROID_MEDIATREE_H
