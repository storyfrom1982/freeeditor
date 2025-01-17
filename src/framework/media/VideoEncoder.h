//
// Created by yongge on 20-2-5.
//

#ifndef ANDROID_VIDEOENCODER_H
#define ANDROID_VIDEOENCODER_H


#include <string>
#include "../message/MessageContext.h"
#include <MediaConfig.h>
#include "../message/MessagePool.h"
#include "../message/MessageChain.h"


namespace freee{

    class VideoEncoder : public MessageChain {

    public:
        static VideoEncoder* Create(std::string name);
        VideoEncoder(const std::string name);
        virtual ~VideoEncoder();

        virtual int OpenEncoder() = 0;
        virtual void CloseEncoder(){};
        virtual int EncoderEncode(Message pkt) = 0;

    protected:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        void onMsgProcessEvent(Message pkt) override;

        void onMsgControl(Message pkt) override;


    private:
        void FinalClear() override ;

    protected:
        float m_frameRate = 0.0f;
        int64_t m_frameId = 0;
        int64_t m_startFrameId = 0;

        size_t m_bufferSize = 0;
        MessagePool *p_bufferPool = nullptr;

    };

}



#endif //ANDROID_VIDEOENCODER_H
