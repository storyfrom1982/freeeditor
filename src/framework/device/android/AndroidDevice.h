//
// Created by yongge on 20-2-2.
//

#ifndef ANDROID_ANDROIDDEVICE_H
#define ANDROID_ANDROIDDEVICE_H

#include <DeviceContext.h>

namespace freee {

    class AndroidDevice : public DeviceContext {

    public:

        static AndroidDevice* Instance();

        ~AndroidDevice() override ;

    protected:

        int onPutObject(int type, void *obj) override;

        void *onGetObject(int type) override;

        int onPutMessage(int cmd, std::string msg) override;

        std::string onGetMessage(int cmd) override;

        int onPutData(void *data, int size) override;

        void *onGetBuffer() override;

    private:

        AndroidDevice(DeviceContext *ctx);
    };
}



#endif //ANDROID_ANDROIDDEVICE_H
