//
// Created by yongge on 20-2-5.
//

#ifndef ANDROID_VIDEOENCODER_H
#define ANDROID_VIDEOENCODER_H


#include <string>
#include <MessageContext.h>
#include <SmartPtr.h>
#include <MConfig.h>
#include <MediaBufferPool.h>
#include "MediaProtocol.h"

namespace freee{

    class VideoEncoder : public MessageContext {

    public:

        static VideoEncoder* Create(std::string name);

        virtual ~VideoEncoder();

        void OpenEncoder(json& cfg);
        void CloseEncoder();
        void EncodeVideo(MediaPacket buffer);

        void SetProtocol(MediaProtocol *aProtocol){
            mediaProtocol = aProtocol;
        }

    protected:

        MediaProtocol *mediaProtocol;

    protected:

        VideoEncoder();

        virtual int OnOpenEncoder(json& cfg) = 0;
        virtual void OnCloseEncoder() = 0;
        virtual void OnEncodeVideo(MediaPacket buffer);
    };

}



#endif //ANDROID_VIDEOENCODER_H
