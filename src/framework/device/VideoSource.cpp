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
    SrMessage b;
    b.frame.key = OpenGLESRender_Init;
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

    sr_buffer_frame_t nv21Pkt = {0};
    sr_buffer_frame_fill(&nv21Pkt, (uint8_t *) data, mInputWidth, mInputHeight, libyuv::FOURCC_NV21);

    sr_buffer_frame_t *y420Pkt;

    SrMessage sb = bp->GetBuffer();
    if (!sb.buffer){
        LOGD("cannot allocate buffer\n");
        return;
    }

    y420Pkt = &sb.frame;
    sr_buffer_frame_fill(y420Pkt, sb.buffer->data, mOutputWidth, mOutputHeight,
                         libyuv::FOURCC_I420);

    sr_buffer_frame_to_yuv420p(&nv21Pkt, y420Pkt, mRotation);

    if (isPreview && window->IsReady()){
        sb.frame.key = OpenGLESRender_DrawPicture;
        render->OnPutMessage(sb);
    }

    encoder->EncodeVideo(sb);
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
    SrMessage b;
    b.frame.key = OpenGLESRender_SetSurfaceView;
    b.frame.data = reinterpret_cast<uint8_t *>(window);
    render->OnPutMessage(b);
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

void VideoSource::updateConfig(sr_message_t msg) {
    json cfg = json::parse(msg.str);
    LOGD("VideoSource::updateConfig: %s\n", cfg.dump(4).c_str());
    mRotation = cfg["rotate"];
    mInputWidth = cfg["pictureWidth"];
    mInputHeight = cfg["pictureHeight"];
    mOutputWidth = cfg["finalWidth"];
    mOutputHeight = cfg["finalHeight"];

    bp = new SrBufferPool(1, mOutputWidth * mOutputHeight / 2 * 3);
//    pool = sr_buffer_pool_create(8);
//    while (sr_buffer_pool_fill(pool, sr_buffer_frame_alloc(mOutputWidth, mOutputHeight,
//                                                           libyuv::FOURCC_I420)) > 0){}
    __sr_msg_clear(msg);
    mConfig["width"] = mOutputWidth;
    mConfig["height"] = mOutputHeight;
    encoder->OpenEncoder(mConfig);
}

