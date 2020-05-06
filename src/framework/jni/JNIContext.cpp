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
        env->EnsureLocalCapacity(16);
        m_obj = env->NewGlobalRef(obj);
        jclass cls = env->GetObjectClass(m_obj);
        m_onRequestMessage = env->GetMethodID(cls, "onRequestMessage", "(I)Lcn/freeeditor/sdk/JNIMessage;");
        m_onReceiveMessage = env->GetMethodID(cls, "onRecvMessage", "(Lcn/freeeditor/sdk/JNIMessage;)V");

        jclass msgCls = env->FindClass("cn/freeeditor/sdk/JNIMessage");
        m_msgCls = (jclass)(env->NewGlobalRef(msgCls));
        m_newJniMessage = env->GetMethodID(m_msgCls, "<init>", "(IIJLjava/lang/String;)V");

        m_keyField = env->GetFieldID(m_msgCls, "key", "I");
        m_ptrField = env->GetFieldID(m_msgCls, "ptr", "J");
        m_stringField = env->GetFieldID(m_msgCls, "string", "Ljava/lang/String;");

        env->DeleteLocalRef(cls);
        env->DeleteLocalRef(msgCls);
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

    void SendMessage(jlong msg){
        MessageContext::SendMessage(Message((sr_msg_t*)msg));
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

    void SendMessage(JNIEnv *env, int key, jstring json){
        unsigned char *js = (unsigned char *)env->GetStringUTFChars(json, 0);
        MessageContext::SendMessage(NewMessage(key, js, env->GetStringUTFLength(json) + 1));
        env->ReleaseStringUTFChars(json, (const char *)js);
    }

    void SendMessage(JNIEnv *env, int key, jint event, jstring json){
        if (json != nullptr){
            unsigned char *js = (unsigned char *)env->GetStringUTFChars(json, 0);
            Message msg = NewMessage(key, js, env->GetStringUTFLength(json) + 1);
            msg.msgType()->i32 = event;
            MessageContext::SendMessage(msg);
            env->ReleaseStringUTFChars(json, (const char *)js);
        }else {
            MessageContext::SendMessage(NewMessage(key, event));
        }
    }

    void SendMessage(int key, jbyte *buffer, jint length){
        Message msg = NewMessage(key);
        msg.msgType()->data = (unsigned char*)buffer;
        msg.msgType()->size = (size_t)(length);
        MessageContext::SendMessage(msg);
    }

    jobject RequestMessage(JNIEnv *env, int key) {
        jstring jstr = nullptr;
        Message msg = MessageContext::RequestMessage(key);
        std::string str = msg.getString();
        if (!str.empty()){
            jstr = env->NewStringUTF(str.c_str());
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newJniMessage, msg.key(), msg.i32(), (jlong) msg.obj(), jstr);
        if (jstr != nullptr){
            env->DeleteLocalRef(jstr);
        }
        return jmsg;
    }

    void onRecvMessage(Message msg) override {
        JniEnv env;
        jstring jstr = nullptr;
        std::string str = msg.getString();
        if (!str.empty()){
            jstr = env->NewStringUTF(str.c_str());
        }
        jobject jmsg = env->NewObject(m_msgCls, m_newJniMessage, msg.key(), msg.i32(), (jlong) msg.obj(), jstr);
        env->CallVoidMethod(m_obj, m_onReceiveMessage, jmsg);
        if (jstr != nullptr){
            env->DeleteLocalRef(jstr);
        }
        env->DeleteLocalRef(jmsg);
    }

    Message onRequestMessage(int key) override {
        JniEnv env;
        jobject jmsg = env->CallObjectMethod(m_obj, m_onRequestMessage, key);
        key = env->GetIntField(jmsg, m_keyField);
        jstring jstr = (jstring)(env->GetObjectField(jmsg, m_stringField));
        if (jstr != nullptr){
            char *s = (char*)env->GetStringUTFChars(jstr, 0);
            Message msg = NewMessage(key, std::string(s, env->GetStringUTFLength(jstr)));
            env->ReleaseStringUTFChars(jstr, s);
            env->DeleteLocalRef(jstr);
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
Java_cn_freeeditor_sdk_JNIContext_deleteContext(JNIEnv *env, jobject instance, jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        delete pJNIContext;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_connectContext__JJ(JNIEnv *env, jobject instance, jlong messageContext,
                                                 jlong contextPointer) {
    MessageContext *pMessageContext = (MessageContext *)(messageContext);
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pMessageContext && pJNIContext){
        pMessageContext->ConnectContext(pJNIContext);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_disconnectContext__J(JNIEnv *env, jobject instance, jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->DisconnectContext();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__ILjava_lang_Object_2J(JNIEnv *env, jobject instance,
                                                                     jint key, jobject obj,
                                                                     jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, obj);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__ILjava_lang_String_2J(JNIEnv *env, jobject instance,
                                                                     jint key, jstring json_,
                                                                     jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(env, key, json_);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IIJ(JNIEnv *env, jobject instance, jint key,
                                                   jint event, jlong contextPointer){
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, event);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__IJJ(JNIEnv *env, jobject instance, jint key,
                                                    jlong ptr,
                                                    jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(key, ptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendMessage__I_3BIJ(JNIEnv *env, jobject instance, jint key,
                                                      jbyteArray buffer_, jint length,
                                                      jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
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
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(env, key, event, json_);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIContext_sendNativeMessage__JJ(JNIEnv *env, jobject instance,
                                                        jlong nativeMessage, jlong contextPointer)
{

    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    if (pJNIContext){
        pJNIContext->SendMessage(nativeMessage);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIContext_requestMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                 jlong contextPointer) {
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    assert(pJNIContext);
    return pJNIContext->RequestMessage(env, key);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_MediaContext_connectMediaContext(JNIEnv *env, jobject instance, jlong contextPointer) {
//    MediaContext::Instance().ConnectContext(reinterpret_cast<MessageContext *>(contextPointer));
    MediaContext::Instance()->ConnectContext((MessageContext *)(contextPointer));
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
        JNIEnv *env, jclass type, jstring tag_, jstring name_, jstring status_) {
    const char *tag = env->GetStringUTFChars(tag_, 0);
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *status = env->GetStringUTFChars(status_, 0);
    LOGD("%s[%p] %40s %s\n", tag, pthread_self(), name, status);
    env->ReleaseStringUTFChars(tag_, tag);
    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(status_, status);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef struct jni_msg_buffer_pool {
    char *name;
    sr_msg_t msg;
    size_t msg_count;
    size_t max_count;
    sr_queue_t *queue;
    bool destroyed;
}jni_msg_buffer_pool_t;

typedef struct jni_msg_buffer_node_t{
    sr_node_t node;
    sr_msg_t msg;
    jni_msg_buffer_pool_t *pool;
}jni_msg_buffer_node_t;


static void jni_release_buffer_node(sr_node_t *node){
    JniEnv env;
    jni_msg_buffer_node_t *buffer_node = (jni_msg_buffer_node_t*)node;
    env->ReleaseByteArrayElements((jbyteArray)buffer_node->msg.buffer.context,
                                  (jbyte*)buffer_node->msg.buffer.head, 0);
    env->DeleteGlobalRef((jbyteArray)buffer_node->msg.buffer.context);
    free(buffer_node);
}

static void jni_buffer_pool_recycle(sr_msg_t *msg)
{
    assert(msg != NULL);
    jni_msg_buffer_node_t *node = (jni_msg_buffer_node_t*)((char *)msg - sizeof(sr_node_t));
    node->msg.type = (sr_msg_type_t){0};
    node->msg.frame = (sr_msg_frame_t){0};
    node->msg.type.data = node->msg.buffer.data;
    __sr_queue_block_push_back(node->pool->queue, node);
    if (__is_true(node->pool->destroyed)){
        jni_msg_buffer_pool_t *pool = node->pool;
        if (sr_queue_length(pool->queue) == pool->msg_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release_delayed() %s [%lu]\n", pool->name, pool->msg_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}

static void jni_msg_pool_realloc(sr_msg_t *msg, size_t size)
{
    assert(msg != NULL);
    JniEnv env;
    if (msg->buffer.context){
        env->ReleaseByteArrayElements((jbyteArray)msg->buffer.context,
                                      (jbyte*)msg->buffer.head, 0);
        env->DeleteGlobalRef((jbyteArray)msg->buffer.context);
    }
    jni_msg_buffer_node_t *node = (jni_msg_buffer_node_t*)((char *)msg - sizeof(sr_node_t));
    assert(node != NULL);
    node->msg.buffer.data_size = size;
    jbyteArray pBuffer = env->NewByteArray(node->msg.buffer.data_size);
    node->msg.buffer.context = env->NewGlobalRef(pBuffer);
    env->DeleteLocalRef(pBuffer);
    node->msg.buffer.head = (unsigned char *)env->GetByteArrayElements(
            (jbyteArray)node->msg.buffer.context, 0);
    assert(node->msg.buffer.head != NULL);
    node->msg.buffer.data = node->msg.buffer.head + msg->buffer.head_size;
    node->msg.type.data = node->msg.buffer.data;
}

jni_msg_buffer_pool_t* jni_msg_buffer_pool_create(JNIEnv *env,
        const char *name,
        size_t msg_count,
        size_t max_count,
        size_t msg_buffer_size,
        size_t msg_buffer_head_size,
        size_t msg_buffer_data_align_size)
{
    jni_msg_buffer_pool_t *pool = (jni_msg_buffer_pool_t*)
            calloc(1, sizeof(jni_msg_buffer_pool_t));
    assert(pool != NULL);
    pool->name = strdup(name);
    pool->msg_count = msg_count;
    pool->max_count = max_count;
    pool->msg.buffer.data_size = msg_buffer_size;
    pool->msg.buffer.head_size = msg_buffer_head_size;
    pool->msg.buffer.align_size = msg_buffer_data_align_size;
    pool->queue = sr_queue_create(jni_release_buffer_node);
    for (int i = 0; i < pool->msg_count; ++i){
        jni_msg_buffer_node_t *node = (jni_msg_buffer_node_t *)calloc(1, sizeof(jni_msg_buffer_node_t));
        assert(node != NULL);
        node->msg = pool->msg;
        jbyteArray pBuffer = env->NewByteArray(node->msg.buffer.data_size);
        node->msg.buffer.context = env->NewGlobalRef(pBuffer);
        env->DeleteLocalRef(pBuffer);
        node->msg.buffer.head = (unsigned char *)env->GetByteArrayElements(
                (jbyteArray)node->msg.buffer.context, 0);
        assert(node->msg.buffer.head != NULL);
        node->msg.buffer.data = node->msg.buffer.head + node->msg.buffer.head_size;
        node->msg.type.data = node->msg.buffer.data;
        node->msg.recycle = jni_buffer_pool_recycle;
        node->msg.realloc = jni_msg_pool_realloc;
        node->pool = pool;
        __sr_queue_push_back(pool->queue, node);
    }
    return pool;
}

void jni_msg_buffer_pool_release(jni_msg_buffer_pool_t **pp_msg_pool){
    if (pp_msg_pool && *pp_msg_pool){
        jni_msg_buffer_pool_t *pool = *pp_msg_pool;
        __set_true(pool->destroyed);
        if (sr_queue_length(pool->queue) == pool->msg_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release() %s [%lu]\n", pool->name, pool->msg_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}

sr_msg_t* jni_msg_buffer_pool_alloc(JNIEnv *env, jni_msg_buffer_pool_t *pool)
{
    assert(pool != NULL);
    if (sr_queue_length(pool->queue) > 0){
        jni_msg_buffer_node_t *node;
        __sr_queue_block_pop_front(pool->queue, node);
        return &node->msg;
    }
    if (pool->msg_count < pool->max_count){
        __sr_atom_add(pool->msg_count, 1);
        jni_msg_buffer_node_t *node = (jni_msg_buffer_node_t *)calloc(1, sizeof(jni_msg_buffer_node_t));
        assert(node != NULL);
        node->msg = pool->msg;
        jbyteArray pBuffer = env->NewByteArray(node->msg.buffer.data_size);
        node->msg.buffer.context = env->NewGlobalRef(pBuffer);
        env->DeleteLocalRef(pBuffer);
        node->msg.buffer.head = (unsigned char *)env->GetByteArrayElements(
                (jbyteArray)node->msg.buffer.context, 0);
        assert(node->msg.buffer.head != NULL);
        node->msg.buffer.data = node->msg.buffer.head + node->msg.buffer.head_size;
        node->msg.type.data = node->msg.buffer.data;
        node->msg.recycle = jni_buffer_pool_recycle;
        node->msg.realloc = jni_msg_pool_realloc;
        node->pool = pool;
        return &node->msg;
    }
    jni_msg_buffer_node_t *node;
    __sr_queue_block_pop_front(pool->queue, node);
    return &node->msg;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_NativeMessagePool_createNativeHandler(JNIEnv *env, jobject instance,
                                                             jstring name_, jint messageCount,
                                                             jint maxMessageCount, jint messageSize)
{
    const char *name = env->GetStringUTFChars(name_, 0);
    jni_msg_buffer_pool_t *pool = jni_msg_buffer_pool_create(env,
            name, messageCount, maxMessageCount, messageSize, 0, 0);
    env->ReleaseStringUTFChars(name_, name);
    return (jlong)pool;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_NativeMessagePool_releaseNativeHandler(JNIEnv *env, jobject instance,
                                                              jlong nativeHandler)
{
    jni_msg_buffer_pool_t *pool = (jni_msg_buffer_pool_t *)nativeHandler;
    jni_msg_buffer_pool_release(&pool);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_NativeMessagePool_newNativeMessage__IJ(JNIEnv *env, jobject instance,
                                                              jint key, jlong nativeHandler)
{
    jni_msg_buffer_pool_t *pool = (jni_msg_buffer_pool_t *)nativeHandler;
    sr_msg_t *msg = jni_msg_buffer_pool_alloc(env, pool);
    if (msg){
        msg->type.key = key;
    }
    return (jlong)msg;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_cn_freeeditor_sdk_NativeMessage_getBuffer__J(JNIEnv *env, jobject instance,
                                                  jlong nativeMessage)
{
    sr_msg_t *msg = (sr_msg_t *)nativeMessage;
    if (msg == nullptr){
        return nullptr;
    }
    return (jbyteArray)msg->buffer.context;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_NativeMessage_releaseMessage(JNIEnv *env, jobject instance,
                                                    jlong nativeMessage)
{
    sr_msg_t *msg = (sr_msg_t *)nativeMessage;
    if (msg != nullptr){
        Message release = Message(msg);
    }
}