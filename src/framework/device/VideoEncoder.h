//
// Created by yongge on 20-2-5.
//

#ifndef ANDROID_VIDEOENCODER_H
#define ANDROID_VIDEOENCODER_H


#include <string>
#include <MessageContext.h>
#include <SmartPtr.h>
#include <MConfig.h>
#include <BufferPool.h>
#include <MediaChainImpl.h>
#include <MediaModule.h>

namespace freee{

    class VideoEncoder : public MediaModule {

    public:
        static VideoEncoder* Create(std::string name);
        virtual ~VideoEncoder();

    protected:
        void MessageOpen(SmartPkt pkt) override;

        void MessageClose(SmartPkt pkt) override;

        void MessageProcessMedia(SmartPkt pkt) override;

        void MessageControl(SmartPkt pkt) override;

    protected:
        VideoEncoder(int mediaType = MediaType_Video,
                int mediaNumber = MediaNumber_VideoEncoder,
                const std::string &mediaName = "VideoEncoder");

    private:
        void FinalClear() override ;

    protected:
        size_t m_bufferSize;
        BufferPool *p_bufferPool;

    };

}



#endif //ANDROID_VIDEOENCODER_H
