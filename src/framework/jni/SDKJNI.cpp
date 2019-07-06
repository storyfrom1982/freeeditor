//
// Created by yongge on 19-5-20.
//

#include "SDKJNI.h"
#include <android/log.h>

#include <MConfig.h>
#include <MContext.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <sr_malloc.h>
#include <sr_library.h>

#ifdef __cplusplus
}
#endif


using namespace freee;


JavaVM *global_JVM_Instance;


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



class MsgHandler : public IMsgListener {

public:

    MsgHandler(jobject obj){
        JniEnv env;
        m_obj = env->NewGlobalRef(obj);
        m_class = env->GetObjectClass(m_obj);
        m_onMessage = env->GetMethodID(m_class, "onMessage", "(Lcn/freeeditor/sdk/Msg;)V");
        m_onRequestMessage = env->GetMethodID(m_class, "onRequest", "(Lcn/freeeditor/sdk/Msg;)Lcn/freeeditor/sdk/Msg;");

        m_msgClass = static_cast<jclass>(env->NewGlobalRef(env->FindClass("cn/freeeditor/sdk/Msg")));
        m_buildInteger = env->GetMethodID(m_msgClass, "<init>", "(IJ)V");
        m_buildFloat = env->GetMethodID(m_msgClass, "<init>", "(ID)V");
        m_buildObject = env->GetMethodID(m_msgClass, "<init>", "(ILjava/lang/Object;I)V");

        m_keyField = env->GetFieldID(m_msgClass, "key", "I");
        m_typeField = env->GetFieldID(m_msgClass, "type", "I");
        m_sizeField = env->GetFieldID(m_msgClass, "size", "I");
        m_int64Field = env->GetFieldID(m_msgClass, "i64", "J");
        m_float64Field = env->GetFieldID(m_msgClass, "f64", "D");
        m_objField = env->GetFieldID(m_msgClass, "obj", "Ljava/lang/Object;");
    };

    ~MsgHandler(){
        JniEnv env;
        if (m_obj != NULL){
            env->DeleteGlobalRef(m_obj);
        }
        if (m_msgClass != NULL){
            env->DeleteGlobalRef(m_msgClass);
        }
    };

    jobject msg2obj(JNIEnv *env, sr_msg_t msg){
        jobject obj;
        if (__sr_msg_is_integer(msg)){
            obj = env->NewObject(m_msgClass, m_buildInteger, msg.key, msg.i64);
        }else if (__sr_msg_is_float(msg)){
            obj = env->NewObject(m_msgClass, m_buildFloat, msg.key, msg.f64);
        }else if (__sr_msg_is_pointer(msg)){
            if (__sr_msg_is_malloc(msg)){
                jbyteArray array = env->NewByteArray(msg.size);
                env->SetByteArrayRegion(array, 0, msg.size, static_cast<const jbyte *>(msg.ptr));
                obj = env->NewObject(m_msgClass, m_buildObject, msg.key, array, msg.size);
                env->DeleteLocalRef(array);
                __sr_msg_free(msg);
            }else {
                obj = env->NewObject(m_msgClass, m_buildObject, msg.key, (jobject)msg.ptr, msg.size);
            }
        }else {
            obj = env->NewObject(m_msgClass, m_buildInteger, -1, -1);
        }
        return obj;
    }

    sr_msg_t obj2msg(JNIEnv *env, jobject obj){
        sr_msg_t msg;
        msg.key = env->GetIntField(obj, m_keyField);
        msg.type = env->GetIntField(obj, m_typeField);
        if (__sr_msg_is_integer(msg)){
            msg.i64 = env->GetLongField(obj, m_int64Field);
        }else if (__sr_msg_is_float(msg)){
            msg.f64 = env->GetDoubleField(obj, m_float64Field);
        }else if (__sr_msg_is_pointer(msg)){
            size_t size = (size_t)(env->GetIntField(obj, m_sizeField));
            if (size > 0){
                jbyteArray array = (jbyteArray)(env->GetObjectField(obj, m_objField));
                LOGD("jbyteArray array : %p\n", array);
                jbyte *bytes = env->GetByteArrayElements(array, 0);
                int byteSize = env->GetArrayLength(array);
                msg = __sr_msg_malloc(msg.key, size + 1);
                msg.size = size;
                memcpy(msg.ptr, bytes, msg.size);
                env->ReleaseByteArrayElements(array, bytes, 0);
                env->DeleteLocalRef(array);
            }else {
                msg.size = 0;
                msg.ptr = env->GetObjectField(obj, m_objField);
            }
        }else {
            msg = __sr_bad_msg;
        }
        return msg;
    }

    jobject requestMessage(JNIEnv *env, jobject obj){
        sr_msg_t msg = obj2msg(env, obj);
        msg = sendRequestToDownstream(msg);
        return msg2obj(env, msg);
    }

    void sendMessage(JNIEnv *env, jobject obj){
        sr_msg_t msg = obj2msg(env, obj);
        sendMessageToDownstream(msg);
    }

    sr_msg_t onRequestFromUpstream(sr_msg_t msg) override {
        JniEnv env;
        jobject obj = msg2obj(env.m_pEnv, msg);
        jobject Msg = env->CallObjectMethod(m_obj, m_onRequestMessage, obj);
        env->DeleteLocalRef(obj);
        return obj2msg(env.m_pEnv, Msg);
    }

    void onMessageFromDownstream(sr_msg_t msg) override {
        JniEnv env;
        jobject obj = msg2obj(env.m_pEnv, msg);
        env->CallVoidMethod(m_obj, m_onMessage, obj);
        env->DeleteLocalRef(obj);
    }


private:

    jobject m_obj;
    jclass m_class;
    jmethodID m_onMessage;
    jmethodID m_onRequestMessage;

    jclass m_msgClass;
    jmethodID m_buildInteger;
    jmethodID m_buildFloat;
    jmethodID m_buildObject;

    jfieldID m_keyField;
    jfieldID m_typeField;
    jfieldID m_sizeField;
    jfieldID m_int64Field;
    jfieldID m_float64Field;
    jfieldID m_objField;
};


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    global_JVM_Instance = vm;
    sr_setup_crash_backtrace();
    sr_log_set_callback(SR_LOG_LEVEL_DEBUG, log_debug);
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_MsgHandler_build(JNIEnv *env, jobject instance) {
    MsgHandler *msgHandler = new MsgHandler(instance);
    return (jlong)(msgHandler);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MsgHandler_remove(JNIEnv *env, jobject instance, jlong handlerInstance) {
    if (handlerInstance != 0){
        MsgHandler *msgHandler = (MsgHandler *)handlerInstance;
        delete msgHandler;
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_MsgHandler_requestMessage(JNIEnv *env, jobject instance, jobject msg, jlong handlerInstance) {
    if (handlerInstance == 0){
        jclass msgClass = static_cast<jclass>(env->NewGlobalRef(env->FindClass("cn/freeeditor/sdk/Msg")));
        jmethodID buildMsg = env->GetMethodID(msgClass, "<init>", "(IJ)V");
        return env->NewObject(msgClass, buildMsg, -1, -1);
    }
    MsgHandler *msgHandler = (MsgHandler *)handlerInstance;
    return msgHandler->requestMessage(env, msg);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MsgHandler_sendMessage(JNIEnv *env, jobject instance, jobject msg, jlong handlerInstance) {
    if (handlerInstance != 0){
        MsgHandler *msgHandler = (MsgHandler *)handlerInstance;
        msgHandler->sendMessage(env, msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MsgHandler_setListener(JNIEnv *env, jobject instance, jlong listener, jlong handlerInstance) {
    IMsgListener *msgHandler = (IMsgListener *)handlerInstance;
    IMsgListener *msgListener = (IMsgListener *)listener;
    if (msgHandler && msgListener){
        msgHandler->addOutputStream(msgListener);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MContext_apply(JNIEnv *env, jobject instance, jlong handlerInstance) {
    if (handlerInstance != 0){
        MsgHandler *msgHandler = (MsgHandler *)handlerInstance;
        MContext::Instance()->addInputStream(msgHandler);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MContext_remove(JNIEnv *env, jobject instance, jlong handlerInstance) {
    if (handlerInstance != 0){
        MsgHandler *msgHandler = (MsgHandler *)handlerInstance;
        msgHandler->removeOutputStream(MContext::Instance());
        MContext::Instance()->removeInputStream(msgHandler);
        delete MContext::Instance();
    }
    sr_log_file_close();
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MContext_debug(JNIEnv *env, jobject instance) {
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