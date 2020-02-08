//
// Created by yongge on 19-8-14.
//

#ifndef ANDROID_STREAMEDITOR_H
#define ANDROID_STREAMEDITOR_H


#include "StreamCapture.h"


namespace freee {

    class StreamEditor : public StreamCapture {

    public:

        StreamEditor(const sr_message_t &msg);

        ~StreamEditor();

    protected:

        virtual void messageFromInputStream(sr_message_t msg);

        virtual void messageFromOutputStream(sr_message_t msg);

        virtual sr_message_t requestFromInputStream(sr_message_t msg);

        virtual sr_message_t requestFromOutputStream(sr_message_t msg);

    };

}


#endif //ANDROID_STREAMEDITOR_H
