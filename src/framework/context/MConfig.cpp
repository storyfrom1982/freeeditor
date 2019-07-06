//
// Created by yongge on 19-5-13.
//

#include "MConfig.h"
#include "MContext.h"

#include <fstream>
#include <iostream>
#include <iomanip>


using namespace freee;



void MConfig::load(json &config, std::string configPath) {
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

            {"videoSource", {
                {"width", 1280},
                {"height", 720},
                {"fps", 25},
                {"position", "front"},
                {"rotation", 0},
                {"mirror", false},
            }},

            {"videoEffect", {
                {"applyList", {"none"}},
            }},

            {"videoEncoder", {
                {"name", "x264"},
                {"bitRate", 1024},
                {"keyFrameInterval", 1},
            }},


            {"audioSource", {
                {"sampleRate", 48000},
                {"channelCount", 1},
                {"bitsPerSample", 16},
                {"position", "center"},
                {"aec", false},
            }},

            {"audioEffect", {
                {"applyList", {"none"}},
            }},

            {"audioEncoder", {
                {"name", "aac"},
                {"bitRate", 128},
            }},
    };
}


//sr_msg_t MConfig::onRequest(sr_msg_t msg) {
//    if (msg.key == MSG_KEY_RequestLoadConfig){
//        std::string cfg = load(MContext::Instance()->getConfigDirPath());
//        size_t size = cfg.length();
//        msg = __sr_msg_build(0, size  +1);
//        memcpy(msg.obj, cfg.c_str(), size);
//        return msg;
//    }else if (msg.key == MSG_KEY_RequestUpdateConfig){
//        if (__sr_msg_is_need_remove(msg)){
//            LOGD("update config: %s\n", (char *)msg.obj);
//            update((char *)msg.obj);
//            __sr_msg_remove(msg);
//        }
//        return __sr_ok_msg;
//    }else if (msg.key == MSG_KEY_RequestSaveConfig){
//        save();
//        return __sr_ok_msg;
//    }
//    return __sr_bad_msg;
//}
