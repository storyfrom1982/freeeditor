//
// Created by yongge on 20-2-1.
//

#include "JNIHandler.h"
#include "DeviceContext.h"
#include "AndroidDevice.h"

#include <android/log.h>

static void log_debug(int level, const char *debug_log, const char *pure_log)
{
    static const char *tag = "FREEESDK";
    if (level == SR_LOG_LEVEL_DEBUG){
        __android_log_print(ANDROID_LOG_DEBUG, tag, "%s", pure_log);
    }else if (level == SR_LOG_LEVEL_INFO){
        __android_log_print(ANDROID_LOG_INFO, tag, "%s", pure_log);
    }else if (level == SR_LOG_LEVEL_WARN){
        __android_log_print(ANDROID_LOG_WARN, tag, "%s", pure_log);
    }else if (level == SR_LOG_LEVEL_ERROR){
        __android_log_print(ANDROID_LOG_ERROR, tag, "%s", pure_log);
    }
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    global_JVM_Instance = vm;
    sr_setup_crash_backtrace();
    sr_log_set_callback(SR_LOG_LEVEL_DEBUG, log_debug);
    return JNI_VERSION_1_6;
}

using namespace freee;

JavaVM *global_JVM_Instance;

class JNIHandler : public DeviceContext {

public:
    JNIHandler(jobject obj, DeviceContext *ctx) : DeviceContext(ctx) {
        JniEnv env;
        m_obj = env->NewGlobalRef(obj);
        jclass m_class = env->GetObjectClass(m_obj);
        m_onPutMessage = env->GetMethodID(m_class, "onPutMessage", "(ILjava/lang/String;)I");
        m_onPutObject = env->GetMethodID(m_class, "onPutObject", "(IJ)I");
        m_onPutData = env->GetMethodID(m_class, "onPutData", "([BI)I");
        m_onGetBuffer = env->GetMethodID(m_class, "onGetBuffer", "()Ljava/nio/ByteBuffer;");
        m_onGetObject = env->GetMethodID(m_class, "onGetObject", "(I)J");
        m_onGetMessage = env->GetMethodID(m_class, "onGetMessage", "(I)Ljava/lang/String;");
        env->DeleteLocalRef(m_class);
    }

    ~JNIHandler() override {
        LOGD("~JNIHandler");
        JniEnv env;
        if (m_obj != NULL){
            env->DeleteGlobalRef(m_obj);
        }
    }

public:
    int onPutObject(int type, void *obj) override {
        JniEnv env;
        return env->CallIntMethod(m_obj, m_onPutObject, type, (jlong)obj);
    }

    void *onGetObject(int type) override {
        JniEnv env;
        return (void*) env->CallLongMethod(m_obj, m_onGetObject, type);
    }

    int onPutMessage(int cmd, std::string msg) override {
        JniEnv env;
        jstring str = env->NewStringUTF(msg.c_str());
        int result = env->CallIntMethod(m_obj, m_onPutMessage, cmd, str);
        env->DeleteLocalRef(str);
        return result;
    }

    std::string onGetMessage(int cmd) override {
        JniEnv env;
        auto s = static_cast<jstring>(env->CallObjectMethod(m_obj, m_onGetMessage, cmd));
        if (s == NULL){
            return "";
        }
        const char *c = env->GetStringUTFChars(s, 0);
        std::string msg(c);
        env->ReleaseStringUTFChars(s, c);
        return msg;
    }

    int onPutData(void *data, int size) override {
        JniEnv env;
        jbyteArray array = env->NewByteArray(size);
        env->SetByteArrayRegion(array, 0, size, (jbyte*)data);
        int result = env->CallIntMethod(m_obj, m_onPutData, array, size);
        env->DeleteLocalRef(array);
        return result;
    }

    void *onGetBuffer() override {
        JniEnv env;
        return env->CallObjectMethod(m_obj, m_onGetBuffer);
    }

private:

    jobject m_obj;
    jmethodID m_onPutMessage;
    jmethodID m_onPutData;
    jmethodID m_onPutObject;
    jmethodID m_onGetMessage;
    jmethodID m_onGetBuffer;
    jmethodID m_onGetObject;
};


extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIHandler_createContext(JNIEnv *env, jobject instance, jlong ctx) {

    DeviceContext *devCtx = reinterpret_cast<DeviceContext *>(ctx);
    JNIHandler *handler = new JNIHandler(instance, devCtx);
    return (jlong)handler;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIHandler_deleteContext(JNIEnv *env, jobject instance, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler){
        delete handler;
    }
}

extern "C"
JNIEXPORT jint JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_putObject__ILjava_lang_Object_2J(
                JNIEnv *env, jobject instance, jint type, jobject obj, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler){
        return handler->putObject(type, obj);
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_putMessage__ILjava_lang_String_2J(
                JNIEnv *env, jobject instance, jint cmd, jstring msg_, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler == NULL){
        return -1;
    }
    const char *msg = env->GetStringUTFChars(msg_, 0);
    int result = handler->putMessage(cmd, msg);
    env->ReleaseStringUTFChars(msg_, msg);
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_putData___3BIJ(
                JNIEnv *env, jobject instance, jbyteArray data_, jint size, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler == NULL){
        return -1;
    }
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int result = handler->putData(data, size);
    env->ReleaseByteArrayElements(data_, data, 0);
    return result;
}

extern "C"
JNIEXPORT jlong JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_getObject__IJ(
                JNIEnv *env, jobject instance, jint type, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler == NULL){
        return 0;
    }
    return (jlong)handler->getObject(type);
}

extern "C"
JNIEXPORT jstring JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_getMessage__IJ(
                JNIEnv *env, jobject instance, jint cmd, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler == NULL){
        return env->NewStringUTF("");
    }
    std::string msg = handler->getMessage(cmd);
    return env->NewStringUTF(msg.c_str());
}

extern "C"
JNIEXPORT jobject JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_getBuffer__J(
                JNIEnv *env, jobject instance, jlong mCtx) {
    JNIHandler *handler = reinterpret_cast<JNIHandler *>(mCtx);
    if (handler == NULL){
        return NULL;
    }
    return static_cast<jobject>(handler->getBuffer());
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIContext_createContext(JNIEnv *env, jobject instance) {
    AndroidDevice *device = AndroidDevice::Instance();
    return (jlong)device;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_deleteContext(JNIEnv *env, jobject instance, jlong mCtx) {
    AndroidDevice *device = (AndroidDevice*)mCtx;
    if (device){
        delete device;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_deleteObject(JNIEnv *env, jobject instance, jlong jniObject) {
    DeviceContext *ctx = (DeviceContext*)jniObject;
    if (ctx){
        delete ctx;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_debug(JNIEnv *env, jobject instance) {
    sr_malloc_debug(sr_log_msg);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_Log_d(JNIEnv *env, jclass type, jstring tag_, jstring msg_) {
    const char *tag = env->GetStringUTFChars(tag_, 0);
    const char *msg = env->GetStringUTFChars(msg_, 0);
    sr_log_debug(SR_LOG_LEVEL_DEBUG, tag, "Java", 0, "%s\n", msg);
    env->ReleaseStringUTFChars(tag_, tag);
    env->ReleaseStringUTFChars(msg_, msg);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_Log_e(JNIEnv *env, jclass type, jstring tag_, jstring msg_) {
    const char *tag = env->GetStringUTFChars(tag_, 0);
    const char *msg = env->GetStringUTFChars(msg_, 0);
    sr_log_debug(SR_LOG_LEVEL_ERROR, tag, "Java", 0, "%s\n", msg);
    env->ReleaseStringUTFChars(tag_, tag);
    env->ReleaseStringUTFChars(msg_, msg);
}