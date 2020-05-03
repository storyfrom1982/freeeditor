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
        m_onRequestMessage = env->GetMethodID(cls, "onRequestMessage1", "(I)J");
        m_onReceiveMessage = env->GetMethodID(cls, "onRecvMessage", "(J)V");

//        jclass msgCls = env->FindClass("cn/freeeditor/sdk/JNIMessage");
//        m_msgCls = (jclass)(env->NewGlobalRef(msgCls));
//        m_newJniMessage = env->GetMethodID(m_msgCls, "<init>", "(IIJLjava/lang/String;)V");
//        m_keyField = env->GetFieldID(m_msgCls, "key", "I");
//        m_ptrField = env->GetFieldID(m_msgCls, "ptr", "J");
//        m_stringField = env->GetFieldID(m_msgCls, "string", "Ljava/lang/String;");
//        env->DeleteLocalRef(msgCls);

        env->DeleteLocalRef(cls);
    }

    ~JNIContext() override {
        JniEnv env;
        if (m_obj != nullptr){
            env->DeleteGlobalRef(m_obj);
        }
//        if (m_msgCls != nullptr){
//            env->DeleteGlobalRef(m_msgCls);
//        }
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
        msg.GetMessagePtr()->length = (size_t)(length);
        MessageContext::SendMessage(msg);
    }

    Message RequestMessage(int key) override {
        return MessageContext::RequestMessage(key);
    }

    void onRecvMessage(Message msg) override {
        JniEnv env;
        sr_buffer_data_t *buffer_data = msg.GetBufferData();
        sr_buffer_data_add_reference(buffer_data);
        env->CallVoidMethod(m_obj, m_onReceiveMessage, buffer_data);
    }

    Message onRequestMessage(int key) override {
        JniEnv env;
        sr_buffer_data_t *buffer_data = (sr_buffer_data_t*)env->CallLongMethod(m_obj, m_onRequestMessage, key);
        Message msg = NewMessage(buffer_data);
        sr_buffer_data_sub_reference(buffer_data);
        return msg;
    }

private:

    jobject m_obj;
    jmethodID m_onReceiveMessage;
    jmethodID m_onRequestMessage;

//    jclass m_msgCls;
//    jmethodID m_newJniMessage;
//    jfieldID m_keyField;
//    jfieldID m_ptrField;
//    jfieldID m_stringField;
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
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIContext_requestMessage__IJ(JNIEnv *env, jobject instance, jint key,
                                                     jlong contextPointer)
{

    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    assert(pJNIContext);
    Message msg = pJNIContext->RequestMessage(key);
    sr_buffer_data_t *buffer_data = msg.GetBufferData();
    sr_buffer_data_add_reference(buffer_data);
    return (jlong)buffer_data;

}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIContext_obtainMessage(JNIEnv *env, jobject instance, jint key,
                                                jlong contextPointer)
{
    JNIContext *pJNIContext = (JNIContext *)(contextPointer);
    assert(pJNIContext);
    Message msg = pJNIContext->NewMessage(key);
    sr_buffer_data_t *buffer_data = msg.GetBufferData();
    sr_buffer_data_add_reference(buffer_data);
    return (jlong)buffer_data;
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

typedef struct sr_buffer_node_t{
    sr_node_t node;
    sr_buffer_data_t buffer;
    sr_buffer_pool_t *pool;
}sr_buffer_node_t;

struct sr_buffer_pool {
    size_t align;
    size_t head_size;
    size_t data_size;
    size_t buffer_count;
    size_t max_buffer_count;
    bool destroyed;
    sr_queue_t *queue;
    char *name;
};

static void release_buffer_node(sr_node_t *node){
    sr_buffer_node_t *buffer_node = (sr_buffer_node_t*)node;
    JniEnv env;
    env->ReleaseByteArrayElements((jbyteArray)buffer_node->buffer.customPtr,
            (jbyte*)buffer_node->buffer.data, 0);
    env->DeleteGlobalRef((jbyteArray)buffer_node->buffer.customPtr);
    free(buffer_node);
}

static void jni_buffer_pool_recycle(sr_buffer_data_t *buffer)
{
    assert(buffer != NULL);
    sr_buffer_node_t *node = (sr_buffer_node_t*)((char *)buffer - sizeof(sr_node_t));
    node->buffer.frame = (sr_buffer_frame_t){0};
    node->buffer.msg = (sr_buffer_message_t){0};
    __sr_queue_block_push_back(node->pool->queue, node);
    if (__is_true(node->pool->destroyed)){
        sr_buffer_pool_t *pool = node->pool;
        if (sr_queue_length(pool->queue) == pool->buffer_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release_delayed() %s [%lu]\n", pool->name, pool->buffer_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}

sr_buffer_pool_t* jni_buffer_pool_create(JNIEnv *env,
        size_t buffer_size,
        size_t buffer_count,
        bool isBlocking,
        const char *name)
{
    sr_buffer_pool_t *pool = (sr_buffer_pool_t*) calloc(1, sizeof(sr_buffer_pool_t));
    assert(pool != NULL);
    pool->data_size = buffer_size;
    pool->buffer_count = buffer_count;
    pool->align = isBlocking;
    pool->name = strdup(name);
    pool->queue = sr_queue_create(release_buffer_node);
    for (int i = 0; i < pool->buffer_count; ++i){
        sr_buffer_node_t *node = (sr_buffer_node_t *)calloc(1, sizeof(sr_buffer_node_t));
        assert(node != NULL);
        node->buffer.data_size = pool->data_size;
        jbyteArray pBuffer = env->NewByteArray(pool->data_size);
        node->buffer.customPtr = env->NewGlobalRef(pBuffer);
        env->DeleteLocalRef(pBuffer);
        node->buffer.data = (unsigned char *)env->GetByteArrayElements((jbyteArray)node->buffer.customPtr, 0);
        node->buffer.recycle = jni_buffer_pool_recycle;
        node->pool = pool;
        __sr_queue_push_back(pool->queue, node);
    }
    return pool;
}

void jni_buffer_pool_release(sr_buffer_pool_t **pp_buffer_pool){
    if (pp_buffer_pool && *pp_buffer_pool){
        sr_buffer_pool_t *pool = *pp_buffer_pool;
        __set_true(pool->destroyed);
        if (sr_queue_length(pool->queue) == pool->buffer_count){
            sr_queue_release(&pool->queue);
            if (pool->name){
                LOGD("sr_buffer_pool_release() %s [%lu]\n", pool->name, pool->buffer_count);
                free(pool->name);
            }
            free(pool);
        }
    }
}

sr_buffer_data_t* jni_buffer_pool_alloc(sr_buffer_pool_t *pool)
{
    assert(pool != NULL);
    sr_buffer_node_t *node = nullptr;
    if (sr_queue_length(pool->queue) > 0){
        __sr_queue_block_pop_front(pool->queue, node);
        return &node->buffer;
    }
    if (pool->align != 0){
        __sr_queue_block_pop_front(pool->queue, node);
        return &node->buffer;
    }
    return nullptr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_jni_MessagePool_createNativeHandler(JNIEnv *env, jobject instance,
                                                           jint messageSize, jint messageCount,
                                                           jboolean isBlocking, jstring name_)
{
    const char *name = env->GetStringUTFChars(name_, 0);
    sr_buffer_pool_t *pool = jni_buffer_pool_create(env, messageSize, messageCount, isBlocking, name);
    env->ReleaseStringUTFChars(name_, name);
    return (jlong)pool;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_jni_MessagePool_releaseNativeHandler(JNIEnv *env, jobject instance,
                                                            jlong nativeHandler)
{
    sr_buffer_pool_t *pool = (sr_buffer_pool_t *)nativeHandler;
    jni_buffer_pool_release(&pool);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_jni_MessagePool_newBufferMessage(JNIEnv *env, jobject instance, jint key,
                                                        jlong nativeHandler)
{
    sr_buffer_pool_t *pool = (sr_buffer_pool_t *)nativeHandler;
    sr_buffer_data_t *buffer = jni_buffer_pool_alloc(pool);
    buffer->msg.key = key;
    sr_buffer_data_add_reference(buffer);
    return (jlong)buffer;
}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getKey__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{

    sr_buffer_data_t *buffer = reinterpret_cast<sr_buffer_data_t *>(nativeMessage);
    if (buffer == NULL){
        return 0;
    }
    return buffer->msg.key;

}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getInt__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    return buffer->msg.event;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getLong__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    return buffer->msg.number;
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getFloat__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{

    // TODO
    return 0;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getDouble__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{

    // TODO
    return 0;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getData__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    jbyteArray array = env->NewByteArray(buffer->msg.length);
    env->SetByteArrayRegion(array, 0, buffer->msg.length, (jbyte*)buffer->data);
    return array;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getBuffer__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    if (buffer->customPtr){
        return (jbyteArray)buffer->customPtr;
    }
    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getObject__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    return (jobject)buffer->msg.customPtr;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_freeeditor_sdk_JNIMessage_getString__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    return env->NewStringUTF((char*)buffer->data);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setKey(JNIEnv *env, jobject instance, jint key,
                                         jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    buffer->msg.key = key;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setInt__IJ(JNIEnv *env, jobject instance, jint i32,
                                             jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    buffer->msg.event = i32;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setLong__JJ(JNIEnv *env, jobject instance, jlong i64,
                                              jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    buffer->msg.number = i64;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setFloat__FJ(JNIEnv *env, jobject instance, jfloat f32,
                                               jlong nativeMessage)
{

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setDouble__DJ(JNIEnv *env, jobject instance, jdouble f64,
                                                jlong nativeMessage)
{

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setData___3BJ(JNIEnv *env, jobject instance, jbyteArray buffer_,
                                                jlong nativeMessage)
{
    jbyte *buffer = env->GetByteArrayElements(buffer_, NULL);
    sr_buffer_data_t *buffer_data = (sr_buffer_data_t *)nativeMessage;
    buffer_data->msg.length = env->GetArrayLength(buffer_);
    memcpy(buffer_data->data, buffer, buffer_data->msg.length);
    env->ReleaseByteArrayElements(buffer_, buffer, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setObject__Ljava_lang_Object_2J(JNIEnv *env, jobject instance,
                                                                  jobject obj, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = (sr_buffer_data_t *)nativeMessage;
    buffer->msg.customPtr = obj;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_setString__Ljava_lang_String_2J(JNIEnv *env, jobject instance,
                                                                  jstring str_, jlong nativeMessage)
{
    const char *str = env->GetStringUTFChars(str_, 0);
    sr_buffer_data_t *buffer_data = (sr_buffer_data_t *)nativeMessage;
    buffer_data->msg.length = env->GetStringUTFLength(str_) + 1;
    memcpy(buffer_data->data, str, buffer_data->msg.length);
    env->ReleaseStringUTFChars(str_, str);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_freeeditor_sdk_JNIMessage_release__J(JNIEnv *env, jobject instance, jlong nativeMessage)
{
    sr_buffer_data_t *buffer = reinterpret_cast<sr_buffer_data_t *>(nativeMessage);
    if (buffer){
        sr_buffer_data_sub_reference(buffer);
    }
}