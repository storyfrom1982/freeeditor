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
        jclass m_class = env->GetObjectClass(m_obj);
        m_onPutMessage = env->GetMethodID(m_class, "onPutMessage", "(ILjava/lang/String;)V");
        m_onPutObject = env->GetMethodID(m_class, "onPutObject", "(ILjava/lang/Object;)V");
        m_onPutContext = env->GetMethodID(m_class, "onPutContext", "(IJ)V");
        m_onGetMessage = env->GetMethodID(m_class, "onGetMessage", "(I)Ljava/lang/String;");
        m_onGetObject = env->GetMethodID(m_class, "onGetObject", "(I)Ljava/lang/Object;");
        m_onGetContext = env->GetMethodID(m_class, "onGetContext", "(I)J");
        env->DeleteLocalRef(m_class);
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
        if (msg.size > MessageType_Command && msg.ptr){
            jstring str = env->NewStringUTF(static_cast<const char *>(msg.ptr));
            env->CallVoidMethod(m_obj, m_onPutMessage, msg.key, str);
            env->DeleteLocalRef(str);
            free(msg.ptr);
        }else if (msg.size == MessageType_Command){
            env->CallVoidMethod(m_obj, m_onPutMessage, msg.key, NULL);
        }else if (msg.size == MessageType_Pointer){
            env->CallVoidMethod(m_obj, m_onPutContext, msg.key, (jlong)msg.ptr);
        }else if (msg.size == MessageType_Unknown){
            env->CallVoidMethod(m_obj, m_onPutObject, msg.key, (jobject)msg.ptr);
        }
    }

    sr_message_t OnGetMessage(sr_message_t msg) override {
        JniEnv env;
        if (msg.size == MessageType_String){
            jstring obj = (jstring)(env->CallObjectMethod(m_obj, m_onGetMessage, msg.key));
            if (obj == nullptr){
                return __sr_null_msg;
            }
            const char *s = env->GetStringUTFChars(obj, 0);
            msg.size = env->GetStringUTFLength(obj);
            msg.ptr = strdup(s);
            env->ReleaseStringUTFChars(obj, s);
        }else if (msg.size == MessageType_Pointer){
            msg.ptr = reinterpret_cast<void *>(env->CallLongMethod(m_obj, m_onGetContext, msg.key));
            if (msg.ptr == nullptr){
                return __sr_null_msg;
            }
        }else if (msg.size == MessageType_Unknown){
            jobject obj = env->CallObjectMethod(m_obj, m_onGetObject, msg.key);
            if (obj == nullptr){
                return __sr_null_msg;
            }
            msg.ptr = env->NewGlobalRef(obj);
        }else {
            return __sr_null_msg;
        }
        return msg;
    }

private:

    jobject m_obj;
    jmethodID m_onPutMessage;
    jmethodID m_onPutContext;
    jmethodID m_onPutObject;
    jmethodID m_onGetMessage;
    jmethodID m_onGetContext;
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
Java_cn_freeeditor_sdk_JNIContext_setContext__JJ(JNIEnv *env, jobject instance, jlong messageContext,
                                                 jlong mCtx) {
    MessageContext *pMessageContext = reinterpret_cast<MessageContext *>(messageContext);
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pMessageContext && pJNIContext){
        pMessageContext->SetContextHandler(pJNIContext);
    }
}

extern "C"
JNIEXPORT void JNICALL
        Java_cn_freeeditor_sdk_JNIContext_putObject__ILjava_lang_Object_2J(
                JNIEnv *env, jobject instance, jint key, jobject obj, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext){
        sr_message_t msg;
        msg.key = key;
        msg.size = MessageType_Unknown;
        LOGD("local obj: %p", obj);
        msg.ptr = env->NewGlobalRef(obj);
        LOGD("global obj: %p", msg.ptr);
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
        Java_cn_freeeditor_sdk_JNIContext_putMessage__ILjava_lang_String_2J(
                JNIEnv *env, jobject instance, jint key, jstring str, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext){
        sr_message_t msg = __sr_null_msg;
        if (str != nullptr){
            const char *s = env->GetStringUTFChars(str, 0);
            msg.key = key;
            msg.size = env->GetStringUTFLength(str);
            msg.ptr = strdup(s);
            env->ReleaseStringUTFChars(str, s);
        }else {
            msg.key = key;
        }
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_putContext__IJJ(JNIEnv *env, jobject instance, jint key,
                                                  jlong messageContext, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext){
        sr_message_t msg;
        msg.key = key;
        msg.size = MessageType_Pointer;
        msg.ptr = reinterpret_cast<void *>(messageContext);
        pJNIContext->PutMessage(msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_putData__I_3BIJ(JNIEnv *env, jobject instance, jint key,
                                                  jbyteArray data_, jint size, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext){
        sr_message_t msg;
        msg.key = key;
        msg.size = size;
        msg.ptr = env->GetByteArrayElements(data_, NULL);
        pJNIContext->PutMessage(msg);
        env->ReleaseByteArrayElements(data_, static_cast<jbyte *>(msg.ptr), 0);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
        Java_cn_freeeditor_sdk_JNIContext_getContext__IJ(
                JNIEnv *env, jobject instance, jint key, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext == NULL){
        return 0;
    }
    sr_message_t msg;
    msg.key = key;
    msg.size = MessageType_Pointer;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key != key){
        return 0;
    }
    return (jlong)msg.ptr;
}

extern "C"
JNIEXPORT jstring JNICALL
        Java_cn_freeeditor_sdk_JNIContext_getMessage__IJ(
                JNIEnv *env, jobject instance, jint key, jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext == nullptr){
        return env->NewStringUTF("");
    }
    sr_message_t msg;
    msg.key = key;
    msg.size = MessageType_String;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key == key && msg.size > MessageType_String){
        jstring str = env->NewStringUTF(static_cast<const char *>(msg.ptr));
        free(msg.ptr);
        return str;
    }
    return env->NewStringUTF("");
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_getObject__IJ(JNIEnv *env, jobject instance, jint key,
                                                jlong mCtx) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(mCtx);
    if (pJNIContext == nullptr){
        return nullptr;
    }
    sr_message_t msg;
    msg.key = key;
    msg.size = MessageType_Unknown;
    msg = pJNIContext->GetMessage(msg);
    if (msg.key == key){
        return (jobject)msg.ptr;
    }
    return nullptr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_MediaContext_createContext(JNIEnv *env, jobject instance) {
    MediaContext *device = MediaContext::Instance();
    return (jlong)device;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_deleteContext(JNIEnv *env, jobject instance, jlong mCtx) {
    MediaContext *device = (MediaContext*)mCtx;
    if (device){
        delete device;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_deleteObject(JNIEnv *env, jobject instance, jlong jniObject) {
    MessageContext *ctx = (MessageContext*)jniObject;
    if (ctx){
        delete ctx;
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