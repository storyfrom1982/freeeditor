//
// Created by yongge on 20-1-21.
//

#include <Device_Bridge.h>

#include <jni.h>
#include "SDKJNI.hbk"

#ifdef __cplusplus
extern "C" {
#endif

#include <sr_malloc.h>
#include <sr_library.h>

#ifdef __cplusplus
}
#endif


struct Device_Bridge{

    void *context;
    size_t (*msg_cb)(void *context, const char *msg, int size);
    int (*data_cb)(void *context, const unsigned char *data, int size);

    jobject jobj;
    jclass jcls;
    jmethodID onRecvMsg;
    jmethodID onRecvData;
};


size_t Device_Bridge_Send_Msg(Device_Bridge *bridge, const char *msg, int size)
{

    JniEnv env;
    jstring str = env->NewStringUTF(msg);
    size_t ret = env->CallLongMethod(bridge->jobj, bridge->onRecvMsg, str);
    env->ReleaseStringUTFChars(str, msg);
    return ret;
}

int Device_Bridge_Send_Data(Device_Bridge *bridge, const unsigned char *data, int size)
{
    JniEnv env;
    jbyteArray array = env->NewByteArray(size);
    env->SetByteArrayRegion(array, 0, size, reinterpret_cast<const jbyte *>(data));
    size_t ret = env->CallLongMethod(bridge->jobj, bridge->onRecvData, array);
    env->DeleteLocalRef(array);
    return 0;
}

void Device_Bridge_Set_Msg_Callback(Device_Bridge *bridge, void *context, size_t (*msg_cb)(void *, const char *, int size))
{
    bridge->context = context;
    bridge->msg_cb = msg_cb;
}

void Device_Bridge_Set_Data_Callback(Device_Bridge *bridge, void *context, int (*data_cb)(void *, const unsigned char *, int size))
{
    bridge->context = context;
    bridge->data_cb = data_cb;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIHandler_createBridge(JNIEnv *env, jobject obj) {

    Device_Bridge *bridge = (Device_Bridge*)malloc(sizeof(Device_Bridge));
    bridge->jobj = env->NewGlobalRef(obj);
    bridge->jcls = env->GetObjectClass(obj);
    bridge->onRecvMsg = env->GetMethodID(bridge->jcls, "onRecvMsg", "(Ljava/lang/String;)J");
    bridge->onRecvData = env->GetMethodID(bridge->jcls, "onRecvData", "([BI)J");

    return (jlong)bridge;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIHandler_removeBridge(JNIEnv *env, jobject instance, jlong jniBridge) {

// TODO

}

extern "C"
JNIEXPORT jlong JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_sendMessage(JNIEnv *env, jobject instance, jstring msg_,
        jlong jniBridge) {
    const char *msg = env->GetStringUTFChars(msg_, 0);
    Device_Bridge *bridge = (Device_Bridge*)jniBridge;
    bridge->msg_cb(bridge->context, msg, env->GetStringUTFLength(msg_));
    env->ReleaseStringUTFChars(msg_, msg);
    return 0;
}

extern "C"
JNIEXPORT jlong JNICALL
        Java_cn_freeeditor_sdk_JNIHandler_sendData(JNIEnv *env, jobject instance, jbyteArray data_,
        jint size, jlong jniBridge) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    Device_Bridge *bridge = (Device_Bridge*)jniBridge;
    bridge->data_cb(bridge->context, reinterpret_cast<const unsigned char *>(data), size);
    env->ReleaseByteArrayElements(data_, data, 0);
    return 0;
}