//
// Created by yongge on 19-5-20.
//

#ifndef ANDROID_MCONTEXT_H
#define ANDROID_MCONTEXT_H

#include <StreamProcessor.h>
#include <Editor.h>

#include <map>


namespace freee {


    enum {
        MsgKey_EnvCtx_HomePath = 3,
        MsgKey_EnvCtx_StoragePath,
        MsgKey_EnvCtx_LoadConfig,
        MsgKey_EnvCtx_SaveConfig,
        MsgKey_EnvCtx_UpdateConfig,
        MsgKey_EnvCtx_SystemInfo,
        MsgKey_EnvCtx_SystemStatus,
        MsgKey_EnvCtx_CreateCapture,
        MsgKey_EnvCtx_RemoveCapture,
        MsgKey_EnvCtx_CreatePlayer,
        MsgKey_EnvCtx_RemovePlayer,
    };


    class EnvContext : public StreamProcessor {

    public:

        static EnvContext* Instance();

        ~EnvContext();

        void addMessageListener(int32_t msgKey, StreamProcessor *processor);

        void removeMessageListener(int32_t msgKey, StreamProcessor *processor);


    protected:

        void messageFromInputStream(sr_msg_t msg) override;

        sr_msg_t requestFromInputStream(sr_msg_t msg) override;


    private:

        EnvContext();


    private:

        Mutex m_processorLock;
        std::map<int32_t, std::vector<StreamProcessor*>> m_processorMap;

    };
}



#endif //ANDROID_MCONTEXT_H
