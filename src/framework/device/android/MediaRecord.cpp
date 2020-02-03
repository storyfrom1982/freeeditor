//
// Created by yongge on 20-2-3.
//

#include "MediaRecord.h"
#include "AndroidDevice.h"


using namespace freee;

MediaRecord::MediaRecord(DeviceContext *ctx) : DeviceContext(ctx) {}

MediaRecord::~MediaRecord() {
    LOGD("MediaRecord::~MediaRecord");
}

int MediaRecord::onPutObject(int type, void *obj) {
    return 0;
}

void *MediaRecord::onGetObject(int type) {
    return nullptr;
}

int MediaRecord::onPutMessage(int cmd, std::string msg) {
    LOGD("MediaRecord::onPutMessage: %s", msg.c_str());
    putMessage(0, "67890");
    return 0;
}

std::string MediaRecord::onGetMessage(int cmd) {
    return std::string();
}

int MediaRecord::onPutData(void *data, int size) {
    return 0;
}

void *MediaRecord::onGetBuffer() {
    return nullptr;
}
