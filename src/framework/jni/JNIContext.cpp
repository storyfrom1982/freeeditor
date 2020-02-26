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
        m_createJniMessage = env->GetMethodID(m_cls, "createJniMessage", "(IJDLjava/lang/Object;Ljava/lang/String;)Lcn/freeeditor/sdk/JNIMessage;");
        env->DeleteLocalRef(m_cls);

        m_msgCls = static_cast<jclass>(env->NewGlobalRef(env->FindClass("cn/freeeditor/sdk/JNIMessage")));
        m_newObject = env->GetMethodID(m_msgCls, "<init>", "(IJDLjava/lang/Object;Ljava/lang/String;)V");

        m_keyField = env->GetFieldID(m_msgCls, "key", "I");
        m_numberField = env->GetFieldID(m_msgCls, "number", "J");
        m_decimalField = env->GetFieldID(m_msgCls, "decimal", "D");
        m_objField = env->GetFieldID(m_msgCls, "obj", "Ljava/lang/Object;");
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

    void SendMessage(int key, jlong number, jdouble decimal){
        SmartPkt pkt(key);
        pkt.msg.number = number;
        pkt.msg.decimal = decimal;
        MessageContext::SendMessage(pkt);
    }

    void SendMessage(int key, jobject obj){
        SmartPkt msg(key);
        msg.msg.troubledPtr = obj;
        MessageContext::SendMessage(msg);
    }

    void SendMessage(int key, jstring json, JNIEnv *env){
        const char *js = env->GetStringUTFChars(json, 0);
        MessageContext::SendMessage(SmartPkt(key, js, env->GetStringUTFLength(json)));
        env->ReleaseStringUTFChars(json, js);
    }

    void SendMessage(int key, jbyte *buffer, int size){
        SmartPkt pkt(key);
        pkt.msg.ptr = buffer;
        pkt.msg.size = size;
        MessageContext::SendMessage(pkt);
    }

//    void SendJNIMessage(jobject jmsg) {
//        JniEnv env;
//        SmartPkt pkt;
//        pkt.msg.key = env->GetIntField(jmsg, m_keyField);
//        pkt.msg.number = env->GetLongField(jmsg, m_numberField);
//        pkt.msg.decimal = env->GetDoubleField(jmsg, m_decimalField);
//        pkt.msg.troubledPtr = env->GetObjectField(jmsg, m_objField);
//        jstring str = static_cast<jstring>(env->GetObjectField(jmsg, m_stringField));
//        if (str != nullptr){
//            const char *js = env->GetStringUTFChars(str, 0);
//            pkt.msg.json = strdup(js);
//            env->ReleaseStringUTFChars(str, js);
//        }
//        MessageContext::SendMessage(pkt);
//    }

    jobject GetJNIMessage(JNIEnv *env, int key) {
        jobject obj = nullptr;
        jstring str = nullptr;
        SmartPkt pkt = MessageContext::GetMessage(key);
        if (pkt.msg.json){
            str = env->NewStringUTF(pkt.msg.json);
        }
        if (pkt.msg.troubledPtr != nullptr){
            obj = static_cast<jobject>(pkt.msg.troubledPtr);
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newObject, pkt.msg.key,
                pkt.msg.number, pkt.msg.decimal, obj, str);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        if (obj != nullptr){
            env->DeleteLocalRef(obj);
        }
        return jmsg;
    }

    void onRecvMessage(SmartPkt pkt) override {
        JniEnv env;
        jobject obj = nullptr;
        jstring str = nullptr;
        if (pkt.msg.json){
            str = env->NewStringUTF(pkt.msg.json);
        }
        if (pkt.msg.troubledPtr != nullptr){
            obj = static_cast<jobject>(pkt.msg.troubledPtr);
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newObject,
                pkt.msg.key, pkt.msg.number, pkt.msg.decimal, obj, str);
        env->CallVoidMethod(m_obj, m_onReceiveMessage, jmsg);
        env->DeleteLocalRef(jmsg);
        if (str != nullptr){
            env->DeleteLocalRef(str);
        }
        if (obj != nullptr){
            env->DeleteLocalRef(obj);
        }
    }

    SmartPkt onObtainMessage(int key) override {
        JniEnv env;
        jobject msg = env->CallObjectMethod(m_obj, m_onObtainMessage, key);
        jstring str = static_cast<jstring>(env->GetObjectField(msg, m_stringField));
        SmartPkt pkt(env->GetIntField(msg, m_keyField));
        pkt.msg.number = env->GetLongField(msg, m_numberField);
        pkt.msg.decimal = env->GetDoubleField(msg, m_decimalField);
        if (str != nullptr){
            const char *js = env->GetStringUTFChars(str, 0);
            pkt.msg.json = strndup(js, env->GetStringUTFLength(str));
            env->ReleaseStringUTFChars(str, js);
        }
        pkt.msg.troubledPtr = env->GetObjectField(msg, m_objField);
        return pkt;
    }

private:

    jobject m_obj;
    jmethodID m_onReceiveMessage;
    jmethodID m_onObtainMessage;
    jmethodID m_createJniMessage;

    jclass m_msgCls;
    jmethodID m_newObject;

    jfieldID m_keyField;
    jfieldID m_stringField;
    jfieldID m_numberField;
    jfieldID m_decimalField;
    jfieldID m_objField;
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

//extern "C"
//JNIEXPORT void JNICALL
//Java_cn_freeeditor_sdk_JNIContext_sendMessage__IJ(JNIEnv *env, jobject instance, jint key,
//                                                 jlong contextPointer) {
//    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
//    if (pJNIContext){
//        pJNIContext->SendMessage(key);
//    }
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_cn_freeeditor_sdk_JNIContext_sendMessage__Lcn_freeeditor_sdk_JNIMessage_2J(JNIEnv *env,
//                                                                                jobject instance,
//                                                                                jobject msg,
//                                                                                jlong contextPointer) {
//    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
//    if (pJNIContext){
//        pJNIContext->SendJNIMessage(msg);
//    }
//}

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
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IJDJ(JNIEnv *env, jobject instance, jint key,
                                                    jlong number, jdouble decimal,
                                                    jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, number, decimal);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__I_3BIJ(JNIEnv *env, jobject instance, jint key,
                                                      jbyteArray buffer_, jint size,
                                                      jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    if (pJNIContext){
        jbyte *buffer = env->GetByteArrayElements(buffer_, NULL);
        pJNIContext->SendMessage(key, buffer, size);
        env->ReleaseByteArrayElements(buffer_, buffer, 0);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_getMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                 jlong contextPointer) {
    JNIContext *pJNIContext = reinterpret_cast<JNIContext *>(contextPointer);
    assert(pJNIContext);
    return pJNIContext->GetJNIMessage(env, key);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_connectMediaContext(JNIEnv *env, jobject instance, jlong contextPointer) {
    MediaContext::Instance().ConnectContext(reinterpret_cast<MessageContext *>(contextPointer));
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