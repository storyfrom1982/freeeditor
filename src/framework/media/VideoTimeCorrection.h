//
// Created by yongge on 20-5-17.
//

#ifndef ANDROID_VIDEOTIMECORRECTION_H
#define ANDROID_VIDEOTIMECORRECTION_H


#include "MediaPlugin.h"

namespace freee {

    class VideoTimeCorrection : public MediaPlugin
    {
    public:
        VideoTimeCorrection(const std::string &name = "VideoTimeCorrection");
        ~VideoTimeCorrection();

    protected:
        void onMsgOpen(Message msg) override;

        void onMsgClose(Message msg) override;

        void onMsgProcessData(Message msg) override;

    private:
        int64_t m_startTime = 0;
        int64_t m_delayTime = 0;
        int64_t m_currentTime = 0;

    };

}


#endif //ANDROID_VIDEOTIMECORRECTION_H
