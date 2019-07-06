//
// Created by yongge on 19-5-20.
//

#ifndef ANDROID_MCONTEXT_H
#define ANDROID_MCONTEXT_H

#include <IMsgListener.h>
#include <Editor.h>

#include <map>


namespace freee {

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

        Editor* createRecorder(sr_msg_t msg);
        void removeRecorder(Editor *recorder);
        Editor* getRecorder(int id = 0);


    private:

        virtual sr_msg_t onRequestFromUpstream(sr_msg_t msg) override ;
        virtual void onMessageFromUpstream(sr_msg_t msg) override ;
        virtual void onMessageFromDownstream(sr_msg_t msg) override ;


    private:

        MContext();


    private:


        Mutex m_pathLock;
        std::string m_homePath;
        std::string m_configPath;

        IMsgListener *m_setPathListener;
        IMsgListener *m_recorderListener;

        Mutex m_recorderLock;
        std::vector<Editor*> m_recorders;

        Mutex m_mapLock;
        std::map<int32_t, std::vector<IMsgListener*>> m_map;

    };
}



#endif //ANDROID_MCONTEXT_H
