//
// Created by yongge on 20-2-1.
//

#include "JNIContext.h"
#include "MessageContext.h"
#include "MediaContext.h"

#include <android/log.h>
#include <BufferPool.h>

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
        m_onObtainMessage = env->GetMethodID(m_cls, "onObtainMessage", "(I)Lcn/freeeditor/sdk/JNIMessage;");
        m_onReceiveMessage = env->GetMethodID(m_cls, "onRecvMessage", "(Lcn/freeeditor/sdk/JNIMessage;)V");
        env->DeleteLocalRef(m_cls);

        m_msgCls = static_cast<jclass>(env->NewGlobalRef(env->FindClass("cn/freeeditor/sdk/JNIMessage")));
        m_newJniMessage = env->GetMethodID(m_msgCls, "<init>", "(IJLjava/lang/String;)V");

        m_keyField = env->GetFieldID(m_msgCls, "key", "I");
        m_ptrField = env->GetFieldID(m_msgCls, "ptr", "J");
        m_stringField = env->GetFieldID(m_msgCls, "string", "Ljava/lang/String;");
    }

    ~JNIContext() override {
        JniEnv env;
        if (m_obj != nullptr){
            env->DeleteGlobalRef(m_obj);
        }
        if (m_msgCls != nullptr){
            env->DeleteGlobalRef(m_msgCls);
        }
    }

    void SendMessage(int key, jlong ptr){
        SmartPkt pkt(key, (void*)ptr);
        MessageContext::SendMessage(pkt);
    }

    void SendMessage(int key, jobject obj){
        SmartPkt pkt(key, obj);
        MessageContext::SendMessage(pkt);
    }

    void SendMessage(int key, jstring json, JNIEnv *env){
        const char *js = env->GetStringUTFChars(json, 0);
        MessageContext::SendMessage(NewJsonPkt(key, std::string(js, env->GetStringUTFLength(json))));
        env->ReleaseStringUTFChars(json, js);
    }

    void SendMessage(int key, jbyte *buffer, jlong timestamp){
        SmartPkt pkt(key);
        pkt.frame.data = (uint8_t*)buffer;
        pkt.frame.timestamp = timestamp;
        MessageContext::SendMessage(pkt);
    }

    jobject GetMessage(JNIEnv *env, int key) {
        jstring str = nullptr;
        SmartPkt pkt = MessageContext::RequestMessage(key);
        if (!pkt.GetString().empty()){
            str = env->NewStringUTF(pkt.GetString().c_str());
        }
        jobject msg = env->NewObject(m_msgCls, m_newJniMessage, pkt.GetKey(), pkt.GetNumber(), str);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        return msg;
    }

    void onRecvMessage(SmartPkt pkt) override {
        JniEnv env;
        jstring str = nullptr;
        if (!pkt.GetString().empty()){
            str = env->NewStringUTF(pkt.GetString().c_str());
        }
        jobject msg = env->NewObject(m_msgCls, m_newJniMessage, pkt.GetKey(), pkt.GetNumber(), str);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        env->CallVoidMethod(m_obj, m_onReceiveMessage, msg);
        env->DeleteLocalRef(msg);
    }

    SmartPkt onObtainMessage(int key) override {
        JniEnv env;
        jobject msg = env->CallObjectMethod(m_obj, m_onObtainMessage, key);
        if (env->GetIntField(msg, m_keyField) == key){
            jstring str = static_cast<jstring>(env->GetObjectField(msg, m_stringField));
            if (str != nullptr){
                return NewJsonPkt(key, std::string(env->GetStringUTFChars(str, 0), env->GetStringUTFLength(str)));
            }else {
                return SmartPkt(key, (void*)env->GetLongField(msg, m_ptrField));
            }
        }
        return SmartPkt();
    }

private:

    jobject m_obj;
    jmethodID m_onReceiveMessage;
    jmethodID m_onObtainMessage;

    jclass m_msgCls;
    jmethodID m_newJniMessage;

    jfieldID m_keyField;
    jfieldID m_ptrField;
    jfieldID m_stringField;
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
Java_cn_freeeditor_sdk_JNIContext_disconnectContext__J(JNIEnv *env, jobject instance, jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->DisconnectContext();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__ILjava_lang_Object_2J(JNIEnv *env, jobject instance,
                                                                     jint key, jobject obj,
                                                                     jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, obj);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__ILjava_lang_String_2J(JNIEnv *env, jobject instance,
                                                                     jint key, jstring json_,
                                                                     jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, json_, env);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IJJ(JNIEnv *env, jobject instance, jint key,
                                                    jlong ptr,
                                                    jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, ptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__I_3BJJ(JNIEnv *env, jobject instance, jint key,
                                                      jbyteArray buffer_, jlong timestamp,
                                                      jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        jbyte *buffer = env->GetByteArrayElements(buffer_, NULL);
        pJNIContext->SendMessage(key, buffer, timestamp);
        env->ReleaseByteArrayElements(buffer_, buffer, 0);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_getMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                 jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    assert(pJNIContext);
    return pJNIContext->GetMessage(env, key);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_connectMediaContext(JNIEnv *env, jobject instance, jlong contextPointer) {
//    MediaContext::Instance().ConnectContext(reinterpret_cast<MessageContext *>(contextPointer));
    MediaContext::Instance()->ConnectContext(reinterpret_cast<MessageContext *>(contextPointer));
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_disconnectMediaContext(JNIEnv *env, jobject instance) {
    MessageContext *context = MediaContext::Instance();
    delete context;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getObject__J(JNIEnv *env, jobject instance, jlong ptr) {
    return (jobject)ptr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getPointer(JNIEnv *env, jobject instance, jobject obj) {
    obj = env->NewGlobalRef(obj);
    return (jlong)obj;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_debug(JNIEnv *env, jclass type) {
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