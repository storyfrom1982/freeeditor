//
// Created by yongge on 19-5-20.
//

#include <vector>
#include "MContext.h"
#include "MConfig.h"


using namespace freee;

static MContext *g_ctx = nullptr;

MContext* MContext::Instance() {
    if (g_ctx == NULL){
        g_ctx = new MContext();
    }
    return g_ctx;
}


MContext::MContext() {

    class SetDirPathListener : public IMsgListener {
        virtual void onMsgFromInput(sr_msg_t msg) override {
            if (__sr_msg_is_pointer(msg)){
                if (__sr_msg_is_malloc(msg)){
                    if (msg.key == MSG_KEY_SetHomeDirPath){
                        MContext::Instance()->setHomeDirPath(std::string(static_cast<const char *>(msg.ptr)));
                    }else if (msg.key == MSG_KEY_SetConfigDirPath){
                        MContext::Instance()->setConfigDirPath(std::string(static_cast<const char *>(msg.ptr)));
                    }
                    __sr_msg_free(msg);
                }
            }
        }
    };

    m_setPathListener = new SetDirPathListener;
    addMessageListener(MSG_KEY_SetHomeDirPath, m_setPathListener);
    addMessageListener(MSG_KEY_SetConfigDirPath, m_setPathListener);

    class RecorderListener : public IMsgListener {
        virtual sr_msg_t onInputRequest(sr_msg_t msg) override {
            if (msg.key == MSG_KEY_RequestNewRecorder){
                Recorder *recorder = MContext::Instance()->createRecorder(msg);
                msg = __sr_ok_msg;
                msg.ptr = recorder;
                return msg;
            }else if (msg.key == MSG_KEY_RequestRemoveRecorder){
                if (msg.ptr != NULL){
                    Recorder *recorder = static_cast<Recorder *>(msg.ptr);
                    MContext::Instance()->removeRecorder(recorder);
                }
                return __sr_ok_msg;
            }
            return __sr_bad_msg;
        }
    };

    m_recorderListener = new RecorderListener;
    addMessageListener(MSG_KEY_RequestNewRecorder, m_recorderListener);
    addMessageListener(MSG_KEY_RequestRemoveRecorder, m_recorderListener);

//    addMessageListener(MSG_KEY_RequestLoadConfig, MConfig::Instance());
//    addMessageListener(MSG_KEY_RequestSaveConfig, MConfig::Instance());
//    addMessageListener(MSG_KEY_RequestUpdateConfig, MConfig::Instance());
};

MContext::~MContext() {
    AutoLock lock(m_mapLock);
    m_map.clear();
//    delete MConfig::Instance();
    if (m_setPathListener){
        delete m_setPathListener;
        m_setPathListener = NULL;
    }
    if (m_recorderListener){
        delete m_recorderListener;
        m_recorderListener = NULL;
    }
    g_ctx = nullptr;
}

sr_msg_t MContext::onInputRequest(sr_msg_t msg) {
    AutoLock lock(m_mapLock);
    if (m_map[msg.key].empty()){
        return __sr_bad_msg;
    }
    return m_map[msg.key][0]->onInputRequest(msg);
}

void MContext::onMsgFromInput(sr_msg_t msg) {
    AutoLock lock(m_mapLock);
    if (!m_map[msg.key].empty()){
        for (int i = 0; i < m_map[msg.key].size(); ++i){
            m_map[msg.key][i]->onMsgFromInput(msg);
        }
    }
}

void MContext::onMsgFromOutput(sr_msg_t msg) {

}

void MContext::sendMessage(sr_msg_t msg) {
    sendMsgToInput(msg);
}

sr_msg_t MContext::requestMessage(sr_msg_t msg) {
    return requestToInput(msg);
}

void MContext::addMessageListener(int32_t key, IMsgListener *listener) {
    AutoLock lock(m_mapLock);
    m_map[key].push_back(listener);
}

void MContext::removeMessageListener(int32_t key, IMsgListener *listener) {
    AutoLock lock(m_mapLock);
    if (!m_map[key].empty()){
        for (int i = 0; i < m_map[key].size(); ++i){
            if (m_map[key][i] == listener){
                m_map[key].erase(m_map[key].begin() + i);
                break;
            }
        }
    }
}

std::string MContext::getConfigDirPath() {
    AutoLock lock(m_dirPathLock);
    return m_configDirPath;
}

std::string MContext::getHomeDirPath() {
    AutoLock lock(m_dirPathLock);
    return m_homeDirPath;
}

void MContext::setHomeDirPath(std::string path) {
    LOGD("setHomeDirPath: %s\n", path.c_str());
    AutoLock lock(m_dirPathLock);
    m_homeDirPath = path;
    sr_log_file_open(m_homeDirPath.c_str());
}

void MContext::setConfigDirPath(std::string path) {
    LOGD("setConfigDirPath: %s\n", path.c_str());
    AutoLock lock(m_dirPathLock);
    m_configDirPath = path;
}

Recorder *MContext::createRecorder(sr_msg_t msg) {
    AutoLock lock(m_recorderLock);
    Recorder *recorder = new Recorder(msg);
    m_recorders.push_back(recorder);
    return recorder;
}

void MContext::removeRecorder(Recorder *recorder) {
    AutoLock lock(m_recorderLock);
    if (!m_recorders.empty()){
        for (int i = 0; i < m_recorders.size(); ++i){
            if (m_recorders[i] == recorder){
                m_recorders.erase(m_recorders.begin() + i);
                delete recorder;
                break;
            }
        }
    }
}

Recorder *MContext::getRecorder(int id) {
    AutoLock lock(m_recorderLock);
    if (id < m_recorders.size()){
        return m_recorders[id];
    }
    return nullptr;
}


