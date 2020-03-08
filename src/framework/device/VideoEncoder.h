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
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessData(SmartPkt pkt) override;

        void onMsgProcessEvent(SmartPkt pkt) override;

    protected:
        void onMsgControl(SmartPkt pkt) override;

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
