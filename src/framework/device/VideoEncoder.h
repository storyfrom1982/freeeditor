//
// Created by yongge on 20-2-5.
//

#ifndef ANDROID_VIDEOENCODER_H
#define ANDROID_VIDEOENCODER_H


#include <string>
#include <MessageContext.h>
#include <SmartPtr.h>
#include <MConfig.h>

namespace freee{

    class VideoEncoder : public MessageContext {

    public:

        static VideoEncoder* Create(std::string name);

        virtual ~VideoEncoder();

        void OpenEncoder(json& cfg);
        void CloseEncoder();
        void EncodeVideo(sr_buffer_t *buffer);

    protected:

        VideoEncoder();

        virtual int OnOpenEncoder(json& cfg) = 0;
        virtual void OnCloseEncoder() = 0;
        virtual void OnEncodeVideo(sr_buffer_t *buffer) = 0;

    };

}



#endif //ANDROID_VIDEOENCODER_H
