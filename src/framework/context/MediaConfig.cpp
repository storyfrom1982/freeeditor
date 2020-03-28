//
// Created by yongge on 19-5-13.
//

#include "MediaConfig.h"

#include <fstream>
#include <iostream>
#include <iomanip>


using namespace freee;



json MediaConfig::load(std::string configPath) {
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

void MediaConfig::save(json &config, std::string configPath) {
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



void MediaConfig::buildDefaultConfig(json &config) {

    config = {

            {CFG_VIDEO, {

                {CFG_TYPE,                          "video"},

                {CFG_SRC_WIDTH,                     1280},
                {CFG_SRC_HEIGHT,                    720},
                {CFG_SRC_IMAGE_FORMAT,              "NV21"},
                {CFG_SRC_VIDEO_DEVICE,              "front"},
                {CFG_SRC_ROTATION,                  0},

                {CFG_CODEC_TYPE,                    "h264"},
                {CFG_CODEC_NAME,                    "x264"},

                {CFG_CODEC_WIDTH,                   1280},
                {CFG_CODEC_HEIGHT,                  720},
                {CFG_CODEC_FRAME_RATE,              15.0},
                {CFG_CODEC_IMAGE_FORMAT,            "I420"},

                {CFG_CODEC_BITRATE,                 512},
                {CFG_CODEC_RATE_CONTROL_MODE,       "VBR"},
                {CFG_CODEC_KEY_FRAME_INTERVAL,      4},

                {CFG_RENDER_MODE,                   "default"},
                {CFG_RENDER_MIRROR,                 false},
                {CFG_RENDER_ROTATION,               0},
            }},


            {CFG_AUDIO, {

                {CFG_TYPE,                          "audio"},

                {CFG_SRC_SAMPLE_RATE,               48000},
                {CFG_SRC_CHANNEL_COUNT,             1},
                {CFG_SRC_SAMPLE_FORMAT,             "S16"},
                {CFG_SRC_BYTE_PER_SAMPLE,           2},
                {CFG_SRC_AUDIO_DEVICE,              "microphone"},

                {CFG_CODEC_TYPE,                    "aac"},
                {CFG_CODEC_NAME,                    "faac"},
                {CFG_CODEC_BITRATE,                 64},
                {CFG_CODEC_SAMPLE_RATE,             48000},
                {CFG_CODEC_CHANNEL_COUNT,           1},
                {CFG_CODEC_SAMPLE_FORMAT,           "S16"},
                {CFG_CODEC_BYTE_PER_SAMPLE,         2},
                {CFG_CODEC_SAMPLE_PER_FRAME,        1024},

                {CFG_SPEAKER_SAMPLE_RATE,           48000},
                {CFG_SPEAKER_SAMPLE_FORMAT,         "S16"},
                {CFG_SPEAKER_CHANNEL_COUNT,         1},
            }},
    };

    json sampleFormat {
            {"U8", "S16", "F32"},
    };

    json imageFormat {
            {"I420", "NV12", "NV21"},
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
