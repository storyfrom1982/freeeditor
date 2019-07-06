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

        MsgKey_Err = -1,
        MsgKey_OK = 0,
        MsgKey_Context_HomePath,
        MsgKey_Context_ConfigPath,
        MsgKey_Context_LoadConfig,
        MsgKey_Context_SaveConfig,
        MsgKey_Context_UpdateConfig,
        MsgKey_Context_SystemInfo,
        MsgKey_Context_SystemStatus,
        MsgKey_Context_NewEditor = 900,
        MsgKey_Context_RemoveEditor,
        MsgKey_Context_NewPlayer,
        MsgKey_Context_RemovePlayer,

        MsgKey_Editor_Begin = 1000,
        MsgKey_Player_Begin = 2000,
        MsgKey_Text_Source_Begin = 3000,
        MsgKey_Text_Effect_Begin = 4000,
        MsgKey_Audio_Source_Begin = 5000,
        MsgKey_Audio_Effect_Begin = 6000,
        MsgKey_Audio_Render_Begin = 7000,
        MsgKey_Video_Source_Begin = 8000,
        MsgKey_Video_Effect_Begin = 9000,
        MsgKey_Video_Render_Begin = 10000,
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

        virtual sr_msg_t onRequestFromUpstream(sr_msg_t msg) override ;
        virtual void onMessageFromUpstream(sr_msg_t msg) override ;
        virtual void onMessageFromDownstream(sr_msg_t msg) override ;


    private:

        MContext();


    private:


        Mutex m_dirPathLock;
        std::string m_homeDirPath;
        std::string m_configDirPath;

        IMsgListener *m_setPathListener;
        IMsgListener *m_recorderListener;

        Mutex m_recorderLock;
        std::vector<Recorder*> m_recorders;

        Mutex m_mapLock;
        std::map<int32_t, std::vector<IMsgListener*>> m_map;

    };
}



#endif //ANDROID_MCONTEXT_H
