//
// Created by yongge on 20-2-1.
//

#include "JNIContext.h"
#include "../message/MessageContext.h"
#include "MediaContext.h"

#include <android/log.h>
#include "../message/MessagePool.h"

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
    JNIContext(const char *name, jobject obj) : MessageContext(name) {
        JniEnv env;
        m_obj = env->NewGlobalRef(obj);
        jclass m_cls = env->GetObjectClass(m_obj);
        m_onRequestMessage = env->GetMethodID(m_cls, "onRequestMessage", "(I)Lcn/freeeditor/sdk/JNIMessage;");
        m_onReceiveMessage = env->GetMethodID(m_cls, "onRecvMessage", "(Lcn/freeeditor/sdk/JNIMessage;)V");
        env->DeleteLocalRef(m_cls);

        m_msgCls = static_cast<jclass>(env->NewGlobalRef(env->FindClass("cn/freeeditor/sdk/JNIMessage")));
        m_newJniMessage = env->GetMethodID(m_msgCls, "<init>", "(IIJLjava/lang/String;)V");

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

    void SendMessage(int key, jint event){
        MessageContext::SendMessage(NewMessage(key, event));
    }

    void SendMessage(int key, jlong ptr){
        MessageContext::SendMessage(NewMessage(key, (void *) ptr));
    }

    void SendMessage(int key, jobject obj){
        MessageContext::SendMessage(NewMessage(key, obj));
    }

    void SendMessage(int key, jstring json, JNIEnv *env){
        unsigned char *js = (unsigned char *)env->GetStringUTFChars(json, 0);
        MessageContext::SendMessage(NewMessage(key, js, env->GetStringUTFLength(json) + 1));
        env->ReleaseStringUTFChars(json, (const char *)js);
    }

    void SendMessage(int key, jint event, jstring json, JNIEnv *env){
        if (json != nullptr){
            unsigned char *js = (unsigned char *)env->GetStringUTFChars(json, 0);
            Message msg = NewMessage(key, js, env->GetStringUTFLength(json) + 1);
            msg.GetMessagePtr()->event = event;
            MessageContext::SendMessage(msg);
            env->ReleaseStringUTFChars(json, (const char *)js);
        }else {
            MessageContext::SendMessage(NewMessage(key, event));
        }
    }

    void SendMessage(int key, jbyte *buffer, jint length){
        Message msg = NewMessage(key);
        msg.GetMessagePtr()->sharePtr = buffer;
        msg.GetMessagePtr()->length = static_cast<size_t>(length);
        MessageContext::SendMessage(msg);
    }

    jobject RequestMessage(JNIEnv *env, int key) {
        jstring str = nullptr;
        Message msg = MessageContext::RequestMessage(key);
        if (!msg.GetString().empty()){
            str = env->NewStringUTF(msg.GetString().c_str());
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newJniMessage, msg.key(), msg.event(), msg.GetNumber(), str);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        return jmsg;
    }

    void onRecvMessage(Message msg) override {
        JniEnv env;
        jstring str = nullptr;
        if (!msg.GetString().empty()){
            str = env->NewStringUTF(msg.GetString().c_str());
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newJniMessage, msg.key(), msg.event(), msg.GetNumber(), str);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        env->CallVoidMethod(m_obj, m_onReceiveMessage, jmsg);
        env->DeleteLocalRef(jmsg);
    }

    Message onRequestMessage(int key) override {
        JniEnv env;
        jobject jmsg = env->CallObjectMethod(m_obj, m_onRequestMessage, key);
        key = env->GetIntField(jmsg, m_keyField);
        jstring str = static_cast<jstring>(env->GetObjectField(jmsg, m_stringField));
        if (str != nullptr){
            char *s = (char*)env->GetStringUTFChars(str, 0);
            Message msg = NewMessage(key, std::string(s, env->GetStringUTFLength(str)));
            env->ReleaseStringUTFChars(str, s);
            env->DeleteLocalRef(str);
            env->DeleteLocalRef(jmsg);
            return msg;
        }
        Message msg = NewMessage(key, (void *) env->GetLongField(jmsg, m_ptrField));
        env->DeleteLocalRef(jmsg);
        return msg;
    }

private:

    jobject m_obj;
    jmethodID m_onReceiveMessage;
    jmethodID m_onRequestMessage;

    jclass m_msgCls;
    jmethodID m_newJniMessage;

    jfieldID m_keyField;
    jfieldID m_ptrField;
    jfieldID m_stringField;
};


extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIContext_createContext__Ljava_lang_String_2(JNIEnv *env, jobject instance,
                                                                     jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);
    JNIContext *pJNIContext = new JNIContext(name, instance);
    env->ReleaseStringUTFChars(name_, name);
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
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IIJ(JNIEnv *env, jobject instance, jint key,
                                                   jint event, jlong contextPointer){
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, event);
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
Java_cn_freeeditor_sdk_JNIContext_sendMessage__I_3BIJ(JNIEnv *env, jobject instance, jint key,
                                                      jbyteArray buffer_, jint length,
                                                      jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        jbyte *buffer = env->GetByteArrayElements(buffer_, NULL);
        pJNIContext->SendMessage(key, buffer, length);
        env->ReleaseByteArrayElements(buffer_, buffer, 0);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IILjava_lang_String_2J(JNIEnv *env,
                                                                       jobject instance, jint key,
                                                                       jint event,
                                                                       jstring json_,
                                                                       jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, event, json_, env);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_requestMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                 jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    assert(pJNIContext);
    return pJNIContext->RequestMessage(env, key);
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

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_Log_dumpThread__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2(
        JNIEnv *env, jclass type, jstring tag_, jstring name_, jstring status_)
{
    const char *tag = env->GetStringUTFChars(tag_, 0);
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *status = env->GetStringUTFChars(status_, 0);

    LOGD("%s[%p] %40s %s\n", tag, pthread_self(), name, status);

    env->ReleaseStringUTFChars(tag_, tag);
    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(status_, status);
}