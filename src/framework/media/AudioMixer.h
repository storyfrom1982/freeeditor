//
// Created by yongge on 20-5-13.
//

#ifndef ANDROID_AUDIOMIXER_H
#define ANDROID_AUDIOMIXER_H


#include "MediaPlugin.h"

namespace freee {


    class AudioMixer : public MediaPlugin
    {
    public:
        AudioMixer(const std::string &name);
        ~AudioMixer();

    protected:
        void onMsgOpen(Message msg) override;

        void onMsgClose(Message msg) override;

        void onMsgProcessData(Message msg) override;

    protected:
        int ProcessMedia(MediaPlugin *plugin, Message msg) override;


    private:
        std::map<MessageChain*, void*> m_channels;

    };

}



#endif //ANDROID_AUDIOMIXER_H
