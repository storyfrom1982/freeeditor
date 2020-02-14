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
    SetContextName("VideoSource");
    MessageContext *sourceContext = MediaContext::Instance()->CreateCamera();
    ConnectContext(sourceContext);
    isPreview = false;
    isClosed = false;
    window = new VideoWindow();
    render = new VideoRenderer();
    sr_message_t msg = __sr_null_msg;
    msg.key = OpenGLESRender_Init;
    render->OnPutMessage(msg);
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
    sr_message_t msg;
    std::string str = mConfig.dump();
    LOGD("VideoSource::Open: %s\n", str.c_str());
    msg.key = VideoSource_Open;
    msg.type = str.length();
    msg.ptr = strdup(str.c_str());
    PutMessage(msg);
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close() {
    LOGD("VideoSource::Close enter\n");
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Close;
    PutMessage(msg);
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start() {
    LOGD("VideoSource::Start enter\n");
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Start;
    PutMessage(msg);
    LOGD("VideoSource::Start exit\n");
}

void VideoSource::Stop() {
    LOGD("VideoSource::Stop enter\n");
    sr_message_t msg = __sr_null_msg;
    msg.key = VideoSource_Stop;
    PutMessage(msg);
    LOGD("VideoSource::Stop exit\n");
}

void VideoSource::SetEncoder(VideoEncoder *videoEncoder) {
    encoder = videoEncoder;
}

void VideoSource::OnPutMessage(sr_message_t msg) {
    switch (msg.key){
        case OnVideoSource_ProcessPicture:
            processData(msg.ptr, msg.type);
            break;
        case OnVideoSource_Opened:
            LOGD("VideoSource::OnPutMessage OnVideoSource_Opened\n");
            updateConfig(msg);
            break;
        case OnVideoSource_Started:
            LOGD("VideoSource::OnPutMessage OnVideoSource_Started\n");
            break;
        case OnVideoSource_Stopped:
            LOGD("VideoSource::OnPutMessage OnVideoSource_Stopped\n");
            break;
        case OnVideoSource_Closed:
            isClosed = true;
            LOGD("VideoSource::OnPutMessage OnVideoSource_Closed\n");
            break;
        default:
            break;
    }
}

sr_message_t VideoSource::OnGetMessage(sr_message_t msg) {
    return MessageContext::OnGetMessage(msg);
}

void VideoSource::processData(void *data, int size) {
    VideoPacket nv21Pkt = {0};
    videoPacket_FillData(&nv21Pkt, (uint8_t*)data, mInputWidth, mInputHeight, libyuv::FOURCC_NV21);

//    sr_message_t *msg = videoEncoder->GetBuffer();
//    if (!msg){
//        return;
//    }

    VideoPacket *y420Pkt;
//    if (msg->ptr == NULL){
//        y420Pkt = videoPacket_Alloc(360, 640, libyuv::FOURCC_I420);
//        msg->ptr = y420Pkt;
//    }else {
//        y420Pkt = static_cast<VideoPacket *>(msg->ptr);
//    }

    sr_buffer_t *buffer = sr_buffer_pool_get(pool);
    if (!buffer){
        LOGD("isPreview:%d  isReady:%d\n", __is_true(isPreview), window->IsReady());
        return;
    }
    y420Pkt = static_cast<VideoPacket *>(buffer->ptr);
    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, mRotation);

    if (isPreview && window->IsReady()){
        sr_message_t msg = __sr_null_msg;
        msg.key = OpenGLESRender_DrawPicture;
        msg.type = MessageType_Pointer;
        msg.ptr = buffer;
        buffer->reference_count ++;
        render->OnPutMessage(msg);
    }

    encoder->EncodeVideo(buffer);
}

void VideoSource::processData(sr_message_t msg) {
//    msg.key = 8;
//    msg.size = MessageType_Pointer;
//    videoEncoder->OnPutDataBuffer(msg);
}

void VideoSource::SetWindow(MessageContext *windowContext) {
    window->ConnectContext(windowContext);
    sr_message_t msg = __sr_null_msg;
    msg.key = OpenGLESRender_SetSurfaceView;
    msg.ptr = window;
    render->OnPutMessage(msg);
}

void VideoSource::StartPreview() {
    isPreview = true;
}

void VideoSource::StopPreview() {
    isPreview = false;
}

void VideoSource::Release() {
    sr_buffer_t *buffer;
    while((buffer = sr_buffer_pool_get(pool)) != NULL){
        VideoPacket *packet = static_cast<VideoPacket *>(buffer->ptr);
        videoPacket_Free(&packet);
    }
    sr_buffer_pool_release(&pool);
}

void VideoSource::updateConfig(sr_message_t msg) {
    json cfg = json::parse(msg.str);
    LOGD("VideoSource::updateConfig: %s\n", cfg.dump(4).c_str());
    mRotation = cfg["rotate"];
    mInputWidth = cfg["pictureWidth"];
    mInputHeight = cfg["pictureHeight"];
    mOutputWidth = cfg["finalWidth"];
    mOutputHeight = cfg["finalHeight"];
    pool = sr_buffer_pool_create(8);
    while (sr_buffer_pool_fill(pool, videoPacket_Alloc(mOutputWidth, mOutputHeight, libyuv::FOURCC_I420)) > 0){}
    __sr_msg_clear(msg);
    mConfig["width"] = mOutputWidth;
    mConfig["height"] = mOutputHeight;
    encoder->OpenEncoder(mConfig);
}

