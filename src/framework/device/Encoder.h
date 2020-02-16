//
// Created by yongge on 20-2-16.
//

#ifndef ANDROID_ENCODER_H
#define ANDROID_ENCODER_H


class MediaObject {

    virtual void Open(MediaObject *obj) = 0;
    virtual void Close(MediaObject *obj) = 0;
    virtual void Start(MediaObject *obj) = 0;
    virtual void Stop(MediaObject *obj) = 0;
    virtual void Control(MediaObject *obj) = 0;
    virtual void ProcessData(MediaObject *obj, void *buffer) = 0;

    virtual void onEvent(MediaObject *object, int event) = 0;
    virtual void onError(MediaObject *object, int error) = 0;
    virtual void onProcessData(MediaObject *object, int error) = 0;

    virtual void GetType(MediaObject *obj) = 0;
    virtual void GetName(MediaObject *obj) = 0;
    virtual void GetConfig(MediaObject *obj) = 0;

};



class Encoder : public MediaObject {

private:
    virtual int OpenEncoder() = 0;
    virtual void CloseEncoder() = 0;
    virtual int EncoderData() = 0;

    void onEvent(int event){

    }

    void onError(int event){

    }

    void onProcessData(void *data){

    }

};



#endif //ANDROID_ENCODER_H
