//
// Created by yongge on 19-5-21.
//

#ifndef ANDROID_SDKJNI_H
#define ANDROID_SDKJNI_H


#include <jni.h>


extern JavaVM*   global_JVM_Instance;

class JniEnv{
public:
    JniEnv(): m_pEnvInternal(NULL){
        global_JVM_Instance->GetEnv((void**)&m_pEnv, JNI_VERSION_1_6 );
        if (m_pEnv == NULL){
            global_JVM_Instance->AttachCurrentThread(&m_pEnvInternal, NULL);
            m_pEnv = m_pEnvInternal;
        }
    }

    ~JniEnv(){
        if (m_pEnvInternal){
            global_JVM_Instance->DetachCurrentThread();
        }
    }

    JNIEnv *operator->(){
        return m_pEnv;
    }
    JNIEnv * m_pEnvInternal;
    JNIEnv * m_pEnv;
};


#endif //ANDROID_SDKJNI_H
