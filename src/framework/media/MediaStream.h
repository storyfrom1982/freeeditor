//
// Created by yongge on 20-3-5.
//

#ifndef ANDROID_MEDIASTREAM_H
#define ANDROID_MEDIASTREAM_H


#include <MediaModule.h>

namespace freee {

    class MediaStream : public MediaModule {
    public:
        MediaStream(int mediaType = MediaType_All,
                int mediaNumber = MediaNumber_OutputProtocal,
                const std::string &mediaName = "MediaStream");
        ~MediaStream();

        void FinalClear() override;

    protected:
        void onMsgOpen(SmartPkt pkt) override;

        void onMsgClose(SmartPkt pkt) override;

        void onMsgProcessMedia(SmartPkt pkt) override;

        void onMsgControl(SmartPkt pkt) override;

    protected:
        std::vector<json> m_configList;
    };

}


#endif //ANDROID_MEDIASTREAM_H
