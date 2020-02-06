//
// Created by yongge on 20-2-2.
//

#include <MConfig.h>
#include "AndroidDevice.h"
#include "MediaRecord.h"

using namespace freee;

AndroidDevice::AndroidDevice(DeviceContext *ctx)
    : DeviceContext(ctx) {}

AndroidDevice::~AndroidDevice() = default;

static AndroidDevice *device = nullptr;

AndroidDevice *AndroidDevice::Instance() {
    if (!device){
        device = new AndroidDevice(nullptr);
    }
    return device;
}

int AndroidDevice::onPutObject(int type, void *obj) {
    return 0;
}

void *AndroidDevice::onGetObject(int type) {
    MediaRecord *record = new MediaRecord(NULL);
    return record;
}

int AndroidDevice::onPutMessage(int cmd, std::string msg) {
    return 0;
}

std::string AndroidDevice::onGetMessage(int cmd) {
    json js;
    MConfig::load(js, "");
    return js.dump();
}

int AndroidDevice::onPutData(void *data, int size) {
    return 0;
}

void *AndroidDevice::onGetBuffer() {
    return nullptr;
}
