//
// Created by yongge on 19-6-16.
//

#include "VideoSource.h"
#include "VideoEncoder.h"


//#ifdef __cplusplus
//extern "C" {
//#endif

#include <libyuv.h>
#include <MediaContext.h>

//#ifdef __cplusplus
//}
//#endif


using namespace freee;


enum {
    VideoSource_Open = 1,
    VideoSource_Start,
    VideoSource_Stop,
    VideoSource_Close,
    VideoSource_Destroy,
};

enum {
    OnVideoSource_Opened = 1,
    OnVideoSource_Started,
    OnVideoSource_Stopped,
    OnVideoSource_Closed,
    OnVideoSource_ProcessPicture,
    OnVideoSource_Destroy,
};

//freee::VideoSource *freee::VideoSource::CreateVideoSource() {
//    MessageContext *sourceContext = MediaContext::Instance()->CreateCamera();
//    VideoSource *videoSource = new VideoSource();
//    videoSource->ConnectContext(sourceContext);
//    return videoSource;
//}

VideoSource::VideoSource() {
//    SetContextName("VideoSource");
    MessageContext *sourceContext = MediaContext::Instance()->CreateCamera();
    ConnectContext(sourceContext);
    isPreview = false;
    isClosed = false;
    window = new VideoWindow();
    render = new VideoRenderer();
    sr_message_t msg = __sr_null_msg;
    msg.key = OpenGLESRender_Init;
    SrPkt b;
    b.msg.key = OpenGLESRender_Init;
    render->OnPutMessage(b);
}

VideoSource::~VideoSource() {
    LOGD("VideoSource::~VideoSource: enter");
    Close();
    while (!isClosed){
        nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
    }
    delete render;
    delete window;
    Release();
//    DisconnectContext();
    LOGD("VideoSource::~VideoSource: exit");
}

void VideoSource::Open(json &cfg) {
    mConfig = cfg;
    SrPkt msg;
    std::string str = mConfig.dump();
    LOGD("VideoSource::Open: %s\n", str.c_str());
    msg.msg.key = VideoSource_Open;
    msg.msg.size = str.length();
    msg.msg.js = strdup(str.c_str());
    SendMessage(msg);
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close() {
    LOGD("VideoSource::Close enter\n");
    SrPkt msg;
    msg.msg.key = VideoSource_Close;
    SendMessage(msg);
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start() {
    LOGD("VideoSource::Start enter\n");
    SrPkt msg;
    msg.msg.key = VideoSource_Start;
    SendMessage(msg);
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop() {
    LOGD("VideoSource::Stop enter\n");
    SrPkt msg;
    msg.msg.key = VideoSource_Stop;
    SendMessage(msg);
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::SetEncoder(VideoEncoder *videoEncoder) {
    encoder = videoEncoder;
}

void VideoSource::onReceiveMessage(SrPkt msg) {
    switch (msg.msg.key){
        case OnVideoSource_ProcessPicture:
            processData(msg.msg.ptr, msg.msg.size);
            break;
        case OnVideoSource_Opened:
            LOGD("VideoSource::onRecvFrom OnVideoSource_Opened\n");
            updateConfig(msg);
            break;
        case OnVideoSource_Started:
            LOGD("VideoSource::onRecvFrom OnVideoSource_Started\n");
            break;
        case OnVideoSource_Stopped:
            LOGD("VideoSource::onRecvFrom OnVideoSource_Stopped\n");
            break;
        case OnVideoSource_Closed:
            isClosed = true;
            LOGD("VideoSource::onRecvFrom OnVideoSource_Closed\n");
            break;
        default:
            break;
    }
}

SrPkt VideoSource::onObtainMessage(int key) {
    return MessageContext::onObtainMessage(key);
}

void VideoSource::processData(void *data, int size) {

    sr_buffer_frame_t nv21Pkt = {0};
    sr_buffer_frame_fill(&nv21Pkt, (uint8_t *) data, mInputWidth, mInputHeight, libyuv::FOURCC_NV21);

    sr_buffer_frame_t *y420Pkt;

    SrPkt pkt = bp->GetBuffer();
    if (!pkt.buffer){
        LOGD("cannot allocate buffer\n");
        return;
    }

    y420Pkt = &pkt.frame;
    sr_buffer_frame_fill(y420Pkt, pkt.buffer->data, mOutputWidth, mOutputHeight,
                         libyuv::FOURCC_I420);

    sr_buffer_frame_to_yuv420p(&nv21Pkt, y420Pkt, mRotation);

    if (isPreview && window->IsReady()){
        pkt.msg.key = OpenGLESRender_DrawPicture;
        render->OnPutMessage(pkt);
    }

    encoder->EncodeVideo(pkt);
}

void VideoSource::SetWindow(MessageContext *windowContext) {
    window->ConnectContext(windowContext);
    SrPkt pkt;
    pkt.msg.key = OpenGLESRender_SetSurfaceView;
    pkt.msg.ptr = window;
    render->OnPutMessage(pkt);
}

void VideoSource::StartPreview() {
    isPreview = true;
}

void VideoSource::StopPreview() {
    isPreview = false;
}

void VideoSource::Release() {
//    sr_buffer_t *buffer;
//    while((buffer = sr_buffer_pool_get(pool)) != NULL){
//        sr_buffer_frame_t *packet = static_cast<sr_buffer_frame_t *>(buffer->ptr);
//        sr_buffer_frame_free(&packet);
//    }
//    sr_buffer_pool_release(&pool);
    delete bp;
}

void VideoSource::updateConfig(SrPkt pkt) {
    json cfg = json::parse(pkt.msg.js);
    LOGD("VideoSource::updateConfig: %s\n", cfg.dump(4).c_str());
    mRotation = cfg["rotate"];
    mInputWidth = cfg["pictureWidth"];
    mInputHeight = cfg["pictureHeight"];
    mOutputWidth = cfg["finalWidth"];
    mOutputHeight = cfg["finalHeight"];

    bp = new SrBufferPool(1, mOutputWidth * mOutputHeight / 2 * 3);
    mConfig["width"] = mOutputWidth;
    mConfig["height"] = mOutputHeight;
    encoder->OpenEncoder(mConfig);
}

