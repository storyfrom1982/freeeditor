//
// Created by yongge on 19-5-24.
//

#include <MContext.h>
#include "Recorder.h"


using namespace freee;


Recorder::Recorder(sr_msg_t msg) : m_videoSource(nullptr) {
    if (__sr_msg_is_pointer(msg) && __sr_msg_is_malloc(msg)){
        m_processor.name = strdup(static_cast<const char *>(msg.ptr));
        __sr_msg_free(msg);
    }else {
        m_processor.name = strdup("DefaultRecorder");
    }
    m_processor.handler = this;
    m_processor.process = Recorder::messageProcessorThread;

    m_queue = sr_msg_queue_build();
    sr_msg_queue_start_processor(m_queue, &m_processor);
}

Recorder::~Recorder() {
    removeVideoSource();
    sr_msg_queue_complete(m_queue);
    sr_msg_queue_remove(&m_queue);
    if (m_processor.name){
        free(m_processor.name);
        m_processor.name = NULL;
    }
}

void Recorder::onMessageFromUpstream(sr_msg_t msg) {
    sr_msg_queue_push(m_queue, msg);
}

void Recorder::onMessageFromDownstream(sr_msg_t msg) {

}

void Recorder::messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg){

    switch (msg.key){
        case MSG_RecvCmd_SetVideoSource:
            setVideoSource(msg);
            break;
        case MSG_RecvCmd_RemoveVideoSource:
            removeVideoSource();
            break;
        case MSG_RecvCmd_SetAudioSource:
            setAudioSource(msg);
            break;
        case MSG_RecvCmd_RemoveAudioSource:
            removeAudioSource();
            break;
        case MSG_RecvCmd_StartPreview:
            startPreview();
            break;
        case MSG_RecvCmd_StopPreview:
            stopPreview();
            break;
        case MSG_RecvCmd_StartPushStream:
            startPushStream();
            break;
        case MSG_RecvCmd_StopPushStream:
            stopPushStream();
            break;
        case MSG_RecvCmd_SetVideoView:
            setVideoView(msg);
            break;
        default:
            break;
    }
}

void Recorder::messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg) {
    static_cast<Recorder *>(processor->handler)->messageProcessorLoop(processor, msg);
}

sr_msg_t Recorder::onRequestFromUpstream(sr_msg_t msg) {

    switch (msg.key){
        case MSG_RecvReq_LoadConfig:
            return loadConfig();
        case MSG_RecvReq_SaveConfig:
            return saveConfig(msg);
        default:
            LOGD("video frame: %p\n", msg.ptr);
            break;
    }

    return __sr_bad_msg;
}

sr_msg_t Recorder::onRequestFromDownstream(sr_msg_t msg) {

    LOGD("video frame: %p\n", msg.ptr);

    return __sr_bad_msg;
}

sr_msg_t Recorder::loadConfig() {
    std::string configPath = MContext::Instance()->getConfigDirPath() + "/" + m_processor.name + ".cfg";
    MConfig::load(m_config, configPath);
    std::string cfg = m_config.dump();
    size_t size = cfg.length();
    sr_msg_t msg = __sr_msg_malloc(0, size  +1);
    memcpy(msg.ptr, cfg.c_str(), size);
    LOGD("load config: %s\n", msg.ptr);
    return msg;
}

sr_msg_t Recorder::saveConfig(sr_msg_t msg) {
    std::string configPath = MContext::Instance()->getConfigDirPath() + "/" + m_processor.name + ".cfg";
    if (__sr_msg_is_pointer(msg) && __sr_msg_is_malloc(msg)){
        m_config.clear();
        m_config.update(json::parse((char *)msg.ptr));
        MConfig::save(m_config, configPath);
        __sr_msg_free(msg);
    }else {
        MConfig::save(m_config, configPath);
    }
    LOGD("save config: %s\n", m_config.dump().c_str());
    return __sr_ok_msg;
}



void Recorder::startPreview() {
    m_videoSource->startCapture();
//    m_videoSource->addOutput(this);
}

void Recorder::stopPreview() {
    m_videoSource->stopCapture();
}

void Recorder::startPushStream() {

}

void Recorder::stopPushStream() {

}

void Recorder::setVideoSource(sr_msg_t msg) {
    if (__sr_msg_is_integer(msg)){
        removeVideoSource();
        m_videoSource = VideoSource::openVideoSource(static_cast<IMsgListener *>(msg.ptr));
        m_videoSource->openSource(m_config["videoSource"]);
    }
}

void Recorder::removeVideoSource() {
    if (m_videoSource){
        m_videoSource->closeSource();
        delete m_videoSource;
        m_videoSource = NULL;
    }
}

void Recorder::setAudioSource(sr_msg_t msg) {

}

void Recorder::removeAudioSource() {

}

void Recorder::setVideoView(sr_msg_t msg) {
    LOGD("setVideoView: %p\n", msg.ptr);
}
