//
// Created by yongge on 19-5-13.
//

#include "MConfig.h"
#include "EnvContext.h"

#include <fstream>
#include <iostream>
#include <iomanip>


using namespace freee;



json MConfig::load(std::string configPath) {
    json config;
    std::ifstream i(configPath);
    if (i){
        LOGD("load %s\n", configPath.c_str());
        i >> config;
        i.close();
    }else {
        if (config.empty()){
            LOGE("load default media config\n");
            buildDefaultConfig(config);
        }
    }
    return config;
}

void MConfig::save(json &config, std::string configPath) {
    if (!config.empty() && !configPath.empty()){
        std::ofstream o(configPath);
        if (o){
            LOGD("save %s\n", configPath.c_str());
            o << std::setw(4) << config << std::endl;
            o.close();
        }else {
            LOGE("save media config failed: %s\n", configPath.c_str());
        }
    }
}


//json &MConfig::get() {
//    if (m_cfg.empty()){
//        buildDefaultConfig();
//    }
//    return m_cfg;
//}
//
//json &MConfig::update(std::string config) {
//    if (!config.empty()){
//        m_cfg.clear();
//        m_cfg.update(json::parse(config));
//    }
//    return m_cfg;
//}
//
//json &MConfig::update(std::string config, std::string objName) {
//    if (!config.empty()){
//        if (m_cfg.find(objName) != m_cfg.end()){
//            m_cfg[objName].update(json::parse(config));
//        }
//        m_cfg[objName] = json::parse(config);
//    }
//    return m_cfg;
//}

void MConfig::buildDefaultConfig(json &config) {

    config = {

            {"video", {

                {"srcWidth", 640},
                {"srcHeight", 360},
                {"srcImageFormat", "NV21"},
                {"srcPosition", "front"},
                {"srcRotation", 0},
                {"srcMirror", false},

                {"previewMode", "default"},
                {"previewMirror", false},
                {"previewRotation", 0},

                {"codecFPS", 25},
                {"codecWidth", 640},
                {"codecHeight", 360},
                {"codecImageFormat", "I420"},
                {"codecType", "h264"},
                {"codecName", "x264"},
                {"codecBitRate", 1024},
                {"codecVBR", true},
                {"codecKeyFrameInterval", 1},
            }},

            {"videoEffect", {
                {"applyList", {"none"}},
            }},


            {"audio", {
                {"srcSampleRate", 48000},
                {"srcChannelCount", 1},
                {"srcBytesPerSample", 2},
                {"srcPosition", "center"},
                {"srcAEC", false},

                {"codecType", "aac"},
                {"codecName", "faac"},
                {"codecBitRate", 128},
                {"codecSampleRate", 48000},
                {"codecChannelCount", 1},
                {"codecBytesPerSample", 2},
                {"codecSamplesPerFrame", 1024},
            }},

            {"audioEffect", {
                {"applyList", {"none"}},
            }},
    };
}


//sr_msg_t MConfig::onReceiveRequest(sr_msg_t msg) {
//    if (msg.key == MSG_KEY_RequestLoadConfig){
//        std::string cfg = load(MContext::Instance()->getConfigDirPath());
//        size_t size = cfg.length();
//        msg = __sr_msg_build(0, size  +1);
//        memcpy(msg.troubledPtr, cfg.c_str(), size);
//        return msg;
//    }else if (msg.key == MSG_KEY_RequestUpdateConfig){
//        if (__sr_msg_is_need_remove(msg)){
//            LOGD("update config: %s\n", (char *)msg.troubledPtr);
//            update((char *)msg.troubledPtr);
//            __sr_msg_remove(msg);
//        }
//        return __sr_ok_msg;
//    }else if (msg.key == MSG_KEY_RequestSaveConfig){
//        save();
//        return __sr_ok_msg;
//    }
//    return __sr_bad_msg;
//}
