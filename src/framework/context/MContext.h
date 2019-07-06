//
// Created by yongge on 19-5-20.
//

#ifndef ANDROID_MCONTEXT_H
#define ANDROID_MCONTEXT_H

#include <IMsgListener.h>
#include <Recorder.h>

#include <map>


namespace freee {

    enum {
        MSG_KEY_RequestSystemInfo = 1,
        MSG_KEY_RequestSystemState = 2,
    };

    enum {
        MSG_KEY_SetHomeDirPath = 1,
        MSG_KEY_SetConfigDirPath = 2,
        MSG_KEY_RequestLoadConfig,
        MSG_KEY_RequestSaveConfig,
        MSG_KEY_RequestUpdateConfig,
        MSG_KEY_RequestNewRecorder,
        MSG_KEY_RequestRemoveRecorder,
        MSG_KEY_RequestNewPlayer,
        MSG_KEY_RequestRemovePlayer,
    };


    class MContext : public IMsgListener {

    public:

        static MContext* Instance();
        ~MContext();

        void sendMessage(sr_msg_t msg);
        sr_msg_t requestMessage(sr_msg_t msg);
        void addMessageListener(int32_t key, IMsgListener *listener);
        void removeMessageListener(int32_t key, IMsgListener *listener);


    public:

        void setHomeDirPath(std::string path);
        void setConfigDirPath(std::string path);

        std::string getHomeDirPath();
        std::string getConfigDirPath();

        Recorder* createRecorder(sr_msg_t msg);
        void removeRecorder(Recorder *recorder);
        Recorder* getRecorder(int id = 0);


    private:

        virtual sr_msg_t onInputRequest(sr_msg_t msg) override ;
        virtual void onMsgFromInput(sr_msg_t msg) override ;
        virtual void onMsgFromOutput(sr_msg_t msg) override ;


    private:

        MContext();


    private:


        StaticMutex m_dirPathLock;
        std::string m_homeDirPath;
        std::string m_configDirPath;

        IMsgListener *m_setPathListener;
        IMsgListener *m_recorderListener;

        StaticMutex m_recorderLock;
        std::vector<Recorder*> m_recorders;

        StaticMutex m_mapLock;
        std::map<int32_t, std::vector<IMsgListener*>> m_map;

    };
}



#endif //ANDROID_MCONTEXT_H
