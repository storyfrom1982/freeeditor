//
// Created by yongge on 20-2-5.
//

#ifndef ANDROID_VIDEOENCODER_H
#define ANDROID_VIDEOENCODER_H


#include <string>
#include <MessageContext.h>
#include <SmartPtr.h>

namespace freee{

    class VideoEncoder {

    public:

        VideoEncoder();
        ~VideoEncoder();

        void openEncoder(std::string config);
        void closeEncoder();
        sr_message_t* GetBuffer();
        void PutBuffer(SmartPtr<sr_buffer_t*> buffer);
        void PutBuffer(sr_buffer_t *buffer);

    private:

        sr_message_queue_t *queue;

    };

}



#endif //ANDROID_VIDEOENCODER_H
