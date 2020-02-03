//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_MEDIARECORD_H
#define ANDROID_MEDIARECORD_H


#include <DeviceContext.h>

namespace freee{

    class MediaRecord : public DeviceContext{

    public:
        MediaRecord(DeviceContext *ctx);
        ~MediaRecord();

    protected:
        int onPutObject(int type, void *obj) override;

        void *onGetObject(int type) override;

        int onPutMessage(int cmd, std::string msg) override;

        std::string onGetMessage(int cmd) override;

        int onPutData(void *data, int size) override;

        void *onGetBuffer() override;
    };
}



#endif //ANDROID_MEDIARECORD_H
