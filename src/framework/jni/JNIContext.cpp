//
// Created by yongge on 20-2-1.
//

#include "JNIContext.h"
#include "MessageContext.h"
#include "MediaContext.h"

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

class JNIContext : public MessageContext {

public:
    JNIContext(jobject obj) {
        JniEnv env;

        m_obj = env->NewGlobalRef(obj);
        jclass m_cls = env->GetObjectClass(m_obj);

        m_onPutMessage = env->GetMethodID(m_cls, "onPutMessage", "(I)V");
        m_onPutLong = env->GetMethodID(m_cls, "onPutLong", "(IJ)V");
        m_onPutJson = env->GetMethodID(m_cls, "onPutJson", "(ILjava/lang/String;)V");
        m_onPutObject = env->GetMethodID(m_cls, "onPutObject", "(ILjava/lang/Object;)V");

        m_onGetJson = env->GetMethodID(m_cls, "onGetJson", "(I)Ljava/lang/String;");
        m_onGetObject = env->GetMethodID(m_cls, "onGetObject", "(I)Ljava/lang/Object;");
        m_onGetLong = env->GetMethodID(m_cls, "onGetLong", "(I)J");

        env->DeleteLocalRef(m_cls);
    }

    ~JNIContext() override {
        JniEnv env;
        if (m_obj != nullptr){
            env->DeleteGlobalRef(m_obj);
        }
    }

    void PutMessage(sr_message_t msg) override {
        MessageContext::PutMessage(msg);
    }

    sr_message_t GetMessage(sr_message_t msg) override {
        return MessageContext::GetMessage(msg);
    }

    void OnPutMessage(sr_message_t msg) override {
        JniEnv env;
        if (__sr_msg_is_none(msg)){
            env->CallVoidMethod(m_obj, m_onPutMessage, msg.key);
        }else if (__sr_msg_is_pointer(msg)){
            env->CallVoidMethod(m_obj, m_onPutLong, msg.key, (jlong)msg.ptr);
        }else if (__sr_msg_is_native(msg)){
            env->CallVoidMethod(m_obj, m_onPutObject, msg.key, (jobject)msg.ptr);
        }else if (__sr_msg_is_json(msg) && msg.str){
            jstring s = env->NewStringUTF(msg.str);
            env->CallVoidMethod(m_obj, m_onPutJson, msg.key, s);
            env->DeleteLocalRef(s);
            free(msg.str);
        }else {
            LOGE("invalid message type\n");
        }
    }

    sr_message_t OnGetMessage(sr_message_t msg) override {
        JniEnv env;
        if (__sr_msg_is_json(msg)){
            jstring obj = (jstring)(env->CallObjectMethod(m_obj, m_onGetJson, msg.key));
            if (obj){
                size_t n = env->GetStringUTFLength(obj);
                const char *s = env->GetStringUTFChars(obj, 0);
                msg.type = env->GetStringUTFLength(obj);
                msg.str = strndup(s, n);
                env->ReleaseStringUTFChars(obj, s);
                return msg;
            }
        }else if (__sr_msg_is_pointer(msg)){
            msg.i64 = (env->CallLongMethod(m_obj, m_onGetLong, msg.key));
            if (msg.ptr){
                return msg;
            }
        }else if (__sr_msg_is_native(msg)){
            jobject obj = env->CallObjectMethod(m_obj, m_onGetObject, msg.key);
            if (obj){
                msg.ptr = obj;
                return msg;
            }

        }else {
            LOGE("invalid message type\n");
        }
        return __sr_null_msg;
    }

private:

    jobject m_obj;
    jmethodID m_onPutMessage;
    jmethodID m_onPutLong;
    jmethodID m_onPutJson;
    jmethodID m_onPutObject;
    jmethodID m_onGetLong;
    jmethodID m_onGetJson;
    jmethodID m_onGetObject;
};


extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIContext_createContext(JNIEnv *env, jobject instance) {
    JNIContext *pJNIContext = new JNIContext(instance);
    return (jlong)pJNIContext;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_deleteContext(JNIEnv *env, jobject instance, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext){
        delete pJNIContext;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_setContextName(JNIEnv *env, jobject instance, jstring name_,
                                                 jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        const char *name = env->GetStringUTFChars(name_, 0);
        pJNIContext->SetContextName(name);
        env->ReleaseStringUTFChars(name_, name);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_connectContext__JJ(JNIEnv *env, jobject instance, jlong messageContext,
                                                 jlong mCtx) {
    MessageContext *pMessageContext = reinterpret_cast<MessageContext *>(messageContext);
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pMessageContext && pJNIContext){
        pMessageContext->ConnectContext(pJNIContext);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_putMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                 jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        sr_message_t msg = __sr_null_msg;
        msg.key = key;
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_putLong__IJJ(JNIEnv *env, jobject instance, jint key,
                                               jlong number, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        sr_message_t msg = __sr_null_msg;
        msg.key = key;
        msg.type = MessageType_Pointer;
        msg.i64 = number;
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
        Java_cn_freeeditor_sdk_JNIContext_putObject__ILjava_lang_Object_2J(
                JNIEnv *env, jobject instance, jint key, jobject obj, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        sr_message_t msg = __sr_null_msg;
        msg.key = key;
        msg.type = MessageType_Native;
        msg.ptr = env->NewGlobalRef(obj);
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
        Java_cn_freeeditor_sdk_JNIContext_putJson__ILjava_lang_String_2J(
                JNIEnv *env, jobject instance, jint key, jstring str, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        sr_message_t msg = __sr_null_msg;
        if (str != nullptr){
            const char *s = env->GetStringUTFChars(str, 0);
            msg.key = key;
            msg.type = env->GetStringUTFLength(str);
            msg.ptr = strdup(s);
            env->ReleaseStringUTFChars(str, s);
            pJNIContext->PutMessage(msg);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_putBuffer__I_3BIJ(JNIEnv *env, jobject instance, jint key,
                                                  jbyteArray data_, jint size, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext){
        sr_message_t msg;
        msg.key = key;
        msg.type = MessageType_Pointer;
        msg.ptr = env->GetByteArrayElements(data_, NULL);
        pJNIContext->PutMessage(msg);
        env->ReleaseByteArrayElements(data_, static_cast<jbyte *>(msg.ptr), 0);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
        Java_cn_freeeditor_sdk_JNIContext_getLong__IJ(
                JNIEnv *env, jobject instance, jint key, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext == NULL){
        return 0;
    }
    sr_message_t msg;
    msg.key = key;
    msg.type = MessageType_Pointer;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key != key){
        return 0;
    }
    return (jlong)msg.ptr;
}

extern "C"
JNIEXPORT jstring JNICALL
        Java_cn_freeeditor_sdk_JNIContext_getJson__IJ(
                JNIEnv *env, jobject instance, jint key, jlong jniContext) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(jniContext);
    if (pJNIContext == nullptr){
        return env->NewStringUTF("");
    }
    sr_message_t msg;
    msg.key = key;
    msg.type = MessageType_Json;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key == key && msg.type > MessageType_None){
        jstring str = env->NewStringUTF(static_cast<const char *>(msg.ptr));
        free(msg.ptr);
        return str;
    }
    return env->NewStringUTF("");
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_getObject__IJ(JNIEnv *jniContext, jobject instance, jint key,
                                                jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext == nullptr){
        return nullptr;
    }
    sr_message_t msg;
    msg.key = key;
    msg.type = MessageType_Native;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key == key){
        return (jobject)msg.ptr;
    }
    return nullptr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_MediaContext_createContext(JNIEnv *env, jobject instance) {
    MediaContext *pMediaContext = MediaContext::Instance();
    return (jlong)pMediaContext;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_deleteContext(JNIEnv *env, jobject instance, jlong mediaContext) {
    MessageContext *pMessageContext = (MessageContext*)mediaContext;
    if (pMessageContext){
        delete pMessageContext;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_debug(JNIEnv *env, jobject instance) {
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