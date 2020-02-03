//
// Created by yongge on 20-2-1.
//

#ifndef ANDROID_DEVICEINTERFACE_H
#define ANDROID_DEVICEINTERFACE_H


#include <string>

#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>

#ifdef __cplusplus
}
#endif


namespace freee{


    class DeviceContext {

    public:

        DeviceContext(DeviceContext *ctx){
            mCtx = ctx;
            if (mCtx){
                mCtx->mCtx = this;
            }
        }

        virtual ~DeviceContext(){};

    public:

        int putObject(int type, void *obj){
            if (!mCtx){
                return -1;
            }
            return mCtx->onPutObject(type, obj);
        }
        void* getObject(int type){
            if (!mCtx){
                return NULL;
            }
            return mCtx->onGetObject(type);
        }
        int putMessage(int cmd, std::string msg){
            if (!mCtx){
                return -1;
            }
            return mCtx->onPutMessage(cmd, msg);
        }
        std::string getMessage(int cmd){
            if (!mCtx){
                return "";
            }
            return mCtx->onGetMessage(cmd);
        }
        int putData(void *data, int size){
            if (!mCtx){
                return -1;
            }
            return mCtx->onPutData(data, size);
        }
        void* getBuffer(){
            if (!mCtx){
                return NULL;
            }
            return mCtx->onGetBuffer();
        }

    protected:

        virtual int onPutObject(int type, void *obj) = 0;
        virtual void* onGetObject(int type) = 0;
        virtual int onPutMessage(int cmd, std::string msg) = 0;
        virtual std::string onGetMessage(int cmd) = 0;
        virtual int onPutData(void *data, int size) = 0;
        virtual void* onGetBuffer() = 0;

        DeviceContext *mCtx;
    };

}



#endif //ANDROID_DEVICEINTERFACE_H
