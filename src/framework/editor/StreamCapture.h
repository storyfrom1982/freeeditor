//
// Created by yongge on 19-8-14.
//

#ifndef ANDROID_CAPTURE_H
#define ANDROID_CAPTURE_H


#include <StreamProcessor.h>


namespace freee{

    class StreamCapture : public StreamProcessor {

    public:

        StreamCapture(sr_msg_t msg){};

        virtual ~StreamCapture(){};

        static void initialize();

        static void release();

        static StreamProcessor* getListener();

        static StreamCapture* create(sr_msg_t msg);


    protected:

        virtual void messageFromInputStream(sr_msg_t msg) override;

        virtual void messageFromOutputStream(sr_msg_t msg) override;

        virtual sr_msg_t requestFromInputStream(sr_msg_t msg) override;

        virtual sr_msg_t requestFromOutputStream(sr_msg_t msg) override;

    };

}



#endif //ANDROID_CAPTURE_H
