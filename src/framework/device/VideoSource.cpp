//
// Created by yongge on 19-6-16.
//

#include "VideoSource.h"
#include "VideoEncoder.h"

#ifdef __ANDROID__
#include <Camera.h>
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <libyuv.h>
#include <android/MediaContext.h>

//#ifdef __cplusplus
//}
//#endif


using namespace freee;

freee::VideoSource *freee::VideoSource::CreateVideoSource() {

    VideoSource *camera = NULL;

#ifdef __ANDROID__
    MessageContext *cameraContext = MediaContext::Instance()->CreateCamera();
    camera = new Camera(cameraContext);
#endif

    return camera;
}

VideoSource::VideoSource() {
    __set_false(isPreview);
    window = new NativeWindow();
    render = new OpenGLESRender();
    sr_message_t msg = __sr_null_msg;
    msg.key = OpenGLESRender_Init;
    render->OnPutMessage(msg);
    pool = sr_buffer_pool_create(2);
    while (sr_buffer_pool_fill(pool, videoPacket_Alloc(360, 640, libyuv::FOURCC_I420)) > 0){}
}

VideoSource::~VideoSource() {
    delete render;
    delete window;
    sr_buffer_t *buffer;
    while((buffer = sr_buffer_pool_get(pool)) != NULL){
        VideoPacket *packet = static_cast<VideoPacket *>(buffer->p);
        videoPacket_Free(&packet);
    }
    sr_buffer_pool_release(&pool);
}

void VideoSource::SetEncoder(VideoEncoder *videoEncoder) {
    encoder = videoEncoder;
}

void VideoSource::processData(void *data, int size) {
    VideoPacket nv21Pkt = {0};
    videoPacket_FillData(&nv21Pkt, (uint8_t*)data, 640, 360, libyuv::FOURCC_NV21);

//    sr_message_t *msg = encoder->GetBuffer();
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
    y420Pkt = static_cast<VideoPacket *>(buffer->p);
    videoPacket_To_YUV420(&nv21Pkt, y420Pkt, 270);

    SmartPtr<sr_buffer_t*> sb(buffer);

    if (isPreview && window->IsReady()){
        sr_message_t msg = __sr_null_msg;
        msg.key = OpenGLESRender_DrawPicture;
        msg.type = MessageType_Pointer;
        msg.ptr = y420Pkt;
        render->OnPutMessage(msg);
    }else {
//        videoPacket_Free(&y420Pkt);
    }

//    encoder->PutBuffer(buffer);
    encoder->PutBuffer(sb);
}

void VideoSource::processData(sr_message_t msg) {
//    msg.key = 8;
//    msg.size = MessageType_Pointer;
//    encoder->OnPutDataBuffer(msg);
}

void VideoSource::SetWindow(MessageContext *windowContext) {
    window->ConnectContextHandler(windowContext);
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

