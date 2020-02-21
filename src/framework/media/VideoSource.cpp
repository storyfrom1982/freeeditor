//
// Created by yongge on 20-2-20.
//

#include <android/MediaContext.h>
#include "VideoSource.h"

using namespace std;
using namespace freee;


enum {
    SendMsg_Open = 1,
    SendMsg_Start,
    SendMsg_Stop,
    SendMsg_Close,
};

enum {
    RecvMsg_Opened = 1,
    RecvMsg_Started,
    RecvMsg_Stopped,
    RecvMsg_Closed,
    RecvMsg_ProcessPicture,
};


VideoSource::VideoSource(MessageContext *context) :
    mModuleName("VideoSource") {
    if (context == nullptr){
        context = MediaContext::Instance().ConnectCamera();
    }
    ConnectContext(context);
}

VideoSource::~VideoSource() {
    LOGD("VideoSource::~VideoSource enter\n");
    SendMessage(MediaPacket(SendMsg_Close));
    DisconnectContext();
    LOGD("VideoSource::~VideoSource exit\n");
}

void VideoSource::Open(MediaChain *chain) {
    LOGD("VideoSource::Open enter\n");
    mConfig = chain->GetMediaConfig(this);
    std::string cfg = mConfig.dump();
    LOGD("VideoSource::Open: %s\n", cfg.c_str());
    MediaPacket pkt(SendMsg_Open);
    pkt.msg.json = strdup(cfg.c_str());
    SendMessage(pkt);
    LOGD("VideoSource::Open exit\n");
}

void VideoSource::Close(MediaChain *chain) {
    LOGD("VideoSource::Close enter\n");
    SendMessage(MediaPacket(SendMsg_Close));
    LOGD("VideoSource::Close exit\n");
}

void VideoSource::Start(MediaChain *chain) {
    LOGD("VideoSource::Close Start\n");
    SendMessage(MediaPacket(SendMsg_Start));
    LOGD("VideoSource::Close Start\n");
}

void VideoSource::Stop(MediaChain *chain) {
    LOGD("VideoSource::Stop Start\n");
    SendMessage(MediaPacket(SendMsg_Stop));
    LOGD("VideoSource::Stop Start\n");
}

void VideoSource::ProcessMedia(MediaChain *chain, MediaPacket pkt) {
    OutputMediaPacket(pkt);
}

int VideoSource::GetMediaType(MediaChain *chain) {
    return MediaType_Video;
}

json &VideoSource::GetMediaConfig(MediaChain *chain) {
    return mConfig;
}

int VideoSource::GetMediaNumber(MediaChain *chain) {
    return MediaNumber_VideoSource;
}

string VideoSource::GetMediaName(MediaChain *chain) {
    return mModuleName;
}

void VideoSource::onReceiveMessage(MediaPacket pkt) {
    switch (pkt.msg.key){
        case RecvMsg_ProcessPicture:
            ProcessMedia(this, pkt);
            break;
        case RecvMsg_Opened:
            LOGD("VideoSource Opened\n");
            onOpened(pkt);
            break;
        case RecvMsg_Started:
            LOGD("VideoSource Started\n");
            break;
        case RecvMsg_Stopped:
            LOGD("VideoSource Stopped\n");
            break;
        case RecvMsg_Closed:
            LOGD("VideoSource Closed\n");
            break;
        default:
            break;
    }
}

void VideoSource::onOpened(MediaPacket pkt) {
    mConfig = json::parse(pkt.msg.json);
}
