//
// Created by yongge on 19-5-24.
//

#include <MContext.h>
#include <MsgKey.h>
#include "Editor.h"


using namespace freee;


Editor::Editor(sr_msg_t msg) : m_videoSource(nullptr) {
    if (__sr_msg_is_pointer(msg) && __sr_msg_is_malloc(msg)){
        m_processor.name = strdup(static_cast<const char *>(msg.ptr));
        __sr_msg_free(msg);
    }else {
        m_processor.name = strdup("DefaultRecorder");
    }
    m_processor.handler = this;
    m_processor.process = Editor::messageProcessorThread;

    m_queue = sr_msg_queue_create();
    sr_msg_queue_start_processor(m_queue, &m_processor);
}

Editor::~Editor() {
    removeVideoSource();
    sr_msg_queue_complete(m_queue);
    sr_msg_queue_remove(&m_queue);
    if (m_processor.name){
        free(m_processor.name);
        m_processor.name = NULL;
    }
}

void Editor::onMessageFromUpstream(sr_msg_t msg) {
    sr_msg_queue_push(m_queue, msg);
}

void Editor::onMessageFromDownstream(sr_msg_t msg) {

}

void Editor::messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg){

    switch (msg.key){
        case MsgKey_Editor_OpenVideoSource:
            setVideoSource(msg);
            break;
        case MsgKey_Editor_CloseVideoSource:
            removeVideoSource();
            break;
        case MsgKey_Editor_OpenAudioSource:
            setAudioSource(msg);
            break;
        case MsgKey_Editor_CloseAudioSource:
            removeAudioSource();
            break;
        case MsgKey_Editor_StartPreview:
            startPreview();
            break;
        case MsgKey_Editor_StopPreview:
            stopPreview();
            break;
        case MsgKey_Editor_StartPublish:
            startPushStream();
            break;
        case MsgKey_Editor_StopPublish:
            stopPushStream();
            break;
        case MsgKey_Editor_SetVideoView:
            setVideoView(msg);
            break;
        default:
            break;
    }
}

void Editor::messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg) {
    static_cast<Editor *>(processor->handler)->messageProcessorLoop(processor, msg);
}

sr_msg_t Editor::onRequestFromUpstream(sr_msg_t msg) {

    switch (msg.key){
        case MsgKey_Editor_LoadConfig:
            return loadConfig();
        case MsgKey_Editor_SaveConfig:
            return saveConfig(msg);
        default:
            LOGD("video frame: %p\n", msg.ptr);
            break;
    }

    return __sr_bad_msg;
}

sr_msg_t Editor::onRequestFromDownstream(sr_msg_t msg) {

    LOGD("video frame: %p\n", msg.ptr);

    return __sr_bad_msg;
}

sr_msg_t Editor::loadConfig() {
    std::string configPath = MContext::Instance()->getConfigDirPath() + "/" + m_processor.name + ".cfg";
    MConfig::load(m_config, configPath);
    std::string cfg = m_config.dump();
    size_t size = cfg.length();
    sr_msg_t msg = __sr_msg_malloc(0, size  +1);
    memcpy(msg.ptr, cfg.c_str(), size);
    LOGD("load config: %s\n", msg.ptr);
    return msg;
}

sr_msg_t Editor::saveConfig(sr_msg_t msg) {
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



void Editor::startPreview() {
    m_videoSource->startCapture();
//    m_videoSource->addOutput(this);
}

void Editor::stopPreview() {
    m_videoSource->stopCapture();
}

void Editor::startPushStream() {

}

void Editor::stopPushStream() {

}

void Editor::setVideoSource(sr_msg_t msg) {
    if (__sr_msg_is_integer(msg)){
        removeVideoSource();
        m_videoSource = VideoSource::openVideoSource(static_cast<IMsgListener *>(msg.ptr));
        m_videoSource->openSource(m_config["videoSource"]);
    }
}

void Editor::removeVideoSource() {
    if (m_videoSource){
        m_videoSource->closeSource();
        delete m_videoSource;
        m_videoSource = NULL;
    }
}

void Editor::setAudioSource(sr_msg_t msg) {

}

void Editor::removeAudioSource() {

}

void Editor::setVideoView(sr_msg_t msg) {
    LOGD("setVideoView: %p\n", msg.ptr);
}
