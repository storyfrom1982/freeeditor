//
// Created by yongge on 19-5-20.
//

#ifndef ANDROID_MAUTOLOCK_H
#define ANDROID_MAUTOLOCK_H


#include <pthread.h>


namespace freee {


    class Mutex {
    public:
        Mutex() : m_mutex(PTHREAD_MUTEX_INITIALIZER){};
        ~Mutex(){}

        void lock(){ pthread_mutex_lock(&m_mutex); }
        void unlock(){ pthread_mutex_unlock(&m_mutex); }

    private:
        pthread_mutex_t m_mutex;
    };


    class AutoLock {
    public:
        AutoLock(Mutex &mutex) : m_lock(mutex){ m_lock.lock(); }
        ~AutoLock(){ m_lock.unlock(); }

    private:
        Mutex &m_lock;
    };

}


#endif //ANDROID_MAUTOLOCK_H
