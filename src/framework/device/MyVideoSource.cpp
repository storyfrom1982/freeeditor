//
// Created by yongge on 19-6-16.
//

#include "MyVideoSource.h"
#include "VideoEncoder.h"


#include <libyuv.h>
#include <MediaContext.h>


using namespace freee;


enum {
    SendMsg_Open = 1,
    SendMsg_Start,
    SendMsg_Stop,
    SendMsg_Close,
};

enum {
    OnRecvMsg_Opened = 1,
    OnRecvMsg_Started,
    OnRecvMsg_Stopped,
    OnRecvMsg_Closed,
    OnRecvMsg_ProcessPicture,
};

//freee::MyVideoSource *freee::MyVideoSource::CreateVideoSource() {
//    MessageContext *sourceContext = MediaContext::Instance()->ConnectCamera();
//    MyVideoSource *videoSource = new MyVideoSource();
//    videoSource->ConnectContext(sourceContext);
//    return videoSource;
//}

MyVideoSource::MyVideoSource() {
//    SetContextName("MyVideoSource");
    MessageContext *sourceContext = MediaContext::Instance().ConnectCamera();
    ConnectContext(sourceContext);
    isPreview = false;
    isClosed = false;
    window = new MyVideoWindow();
    render = new MyVideoRenderer();
    sr_message_t msg = __sr_null_msg;
    msg.key = OpenGLESRender_Init;
    SmartPkt b;
    b.msg.key = OpenGLESRender_Init;
    render->OnPutMessage(b);
}

MyVideoSource::~MyVideoSource() {
    LOGD("MyVideoSource::MyVideoSourcece: enter");
    Close();
    while (!isClosed){
        nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
    }
    delete render;
    delete window;
    Release();
//    DisconnectContext();
    LOGD("MyVideoSource::MyVideoSourcece: exit");
}

void MyVideoSource::Open(json &cfg) {
    mConfig = cfg;
    SmartPkt msg;
    std::string str = mConfig.dump();
    LOGD("MyVideoSource::Open: %s\n", str.c_str());
    msg.msg.key = SendMsg_Open;
    msg.msg.size = str.length();
    msg.msg.json = strdup(str.c_str());
    SendMessage(msg);
    LOGD("MyVideoSource::Open exit\n");
}

void MyVideoSource::Close() {
    LOGD("MyVideoSource::Close enter\n");
    SmartPkt msg;
    msg.msg.key = SendMsg_Close;
    SendMessage(msg);
    LOGD("MyVideoSource::Close exit\n");
}

void MyVideoSource::Start() {
    LOGD("MyVideoSource::Start enter\n");
    SmartPkt msg;
    msg.msg.key = SendMsg_Start;
    SendMessage(msg);
    LOGD("MyVideoSource::Start exit\n");
}

void MyVideoSource::Stop() {
    LOGD("MyVideoSource::Stop enter\n");
    SmartPkt msg;
    msg.msg.key = SendMsg_Stop;
    SendMessage(msg);
    LOGD("MyVideoSource::Stop exit\n");
}

void MyVideoSource::SetEncoder(VideoEncoder *videoEncoder) {
    AutoLock lock(mLock);
    encoder = videoEncoder;
}

void MyVideoSource::onRecvMessage(SmartPkt msg) {
    switch (msg.msg.key){
        case OnRecvMsg_ProcessPicture:
            processData(msg.msg.ptr, msg.msg.size);
            break;
        case OnRecvMsg_Opened:
            LOGD("MyVideoSource::onRecvFrom OnRecvMsg_Opened\n");
            updateConfig(msg);
            break;
        case OnRecvMsg_Started:
            LOGD("MyVideoSource::onRecvFrom OnRecvMsg_Started\n");
            break;
        case OnRecvMsg_Stopped:
            LOGD("MyVideoSource::onRecvFrom OnVideoSource_Stopped\n");
            break;
        case OnRecvMsg_Closed:
            isClosed = true;
            LOGD("MyVideoSource::onRecvFrom OnVideoSource_Closed\n");
            break;
        default:
            break;
    }
}

SmartPkt MyVideoSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void MyVideoSource::processData(void *data, int size) {

    AutoLock lock(mLock);

    sr_buffer_frame_t nv21Pkt = {0};
    sr_buffer_frame_fill_picture(&nv21Pkt, (uint8_t *) data, mInputWidth, mInputHeight,
                                 libyuv::FOURCC_NV21);

    sr_buffer_frame_t *y420Pkt;

    SmartPkt pkt = bp->GetPkt();
    if (!pkt.buffer){
        LOGD("cannot allocate buffer\n");
        return;
    }

    y420Pkt = &pkt.frame;
    sr_buffer_frame_fill_picture(y420Pkt, pkt.buffer->data, mOutputWidth, mOutputHeight,
                                 libyuv::FOURCC_I420);

    sr_buffer_frame_to_yuv420p(&nv21Pkt, y420Pkt, mRotation);

    if (isPreview && window->IsReady()){
        pkt.msg.key = OpenGLESRender_DrawPicture;
        render->OnPutMessage(pkt);
    }

    encoder->EncodeVideo(pkt);
}

void MyVideoSource::SetWindow(MessageContext *windowContext) {
    AutoLock lock(mLock);
    window->ConnectContext(windowContext);
    SmartPkt pkt;
    pkt.msg.key = OpenGLESRender_SetSurfaceView;
    pkt.msg.ptr = window;
    render->OnPutMessage(pkt);
}

void MyVideoSource::StartPreview() {
    isPreview = true;
}

void MyVideoSource::StopPreview() {
    isPreview = false;
}

void MyVideoSource::Release() {
    if (bp){
        delete bp;
        bp = nullptr;
    }
}

void MyVideoSource::updateConfig(SmartPkt pkt) {
    json cfg = json::parse(pkt.msg.json);
    LOGD("MyVideoSource::updateConfig: %s\n", cfg.dump(4).c_str());
    mRotation = cfg["srcRotation"];
    mInputWidth = cfg["srcWidth"];
    mInputHeight = cfg["srcHeight"];
    mOutputWidth = cfg["codecWidth"];
    mOutputHeight = cfg["codecHeight"];
    bp = new BufferPool(1, mOutputWidth * mOutputHeight / 2 * 3);
}

