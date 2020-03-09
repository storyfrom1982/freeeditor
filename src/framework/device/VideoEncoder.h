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
#include <MessageChainImpl.h>
#include <MediaModule.h>


namespace freee{

    class VideoEncoder : public MediaModule {

    public:
        static VideoEncoder* Create(std::string name);
        virtual ~VideoEncoder();

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

    protected:
        void onMsgControl(Message pkt) override;

    protected:
        VideoEncoder(int mediaType = MediaType_Video,
                int mediaNumber = MediaNumber_VideoEncoder,
                const std::string &mediaName = "VideoEncoder");

    private:
        void FinalClear() override ;

    protected:
        float m_frameRate = 0.0f;
        int64_t m_frameId = 0;
        int64_t m_startFrameId = 0;

        size_t m_bufferSize = 0;
        BufferPool *p_bufferPool = nullptr;

    };

}



#endif //ANDROID_VIDEOENCODER_H
