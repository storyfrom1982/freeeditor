//
// Created by yongge on 19-5-20.
//

#ifndef ANDROID_MAUTOLOCK_H
#define ANDROID_MAUTOLOCK_H


#include <pthread.h>


namespace freee {


    class Lock {
    public:
        Lock() : m_mutex(PTHREAD_MUTEX_INITIALIZER), m_cond(PTHREAD_COND_INITIALIZER){};
        ~Lock(){}

        void lock(){
            pthread_mutex_lock(&m_mutex);
        }
        void unlock(){
            pthread_mutex_unlock(&m_mutex);
        }
        void signal(){
            pthread_cond_signal(&m_cond);
        }
        void broadcast(){
            pthread_cond_broadcast(&m_cond);
        }
        void wait(){
            pthread_cond_wait(&m_cond, &m_mutex);
        }

    private:
        pthread_cond_t m_cond;
        pthread_mutex_t m_mutex;
    };


    class AutoLock {
    public:
        AutoLock(Lock &lock) : m_lock(lock){
            m_lock.lock();
        }
        void signal(){
            m_lock.signal();
        }
        void broadcast(){
            m_lock.broadcast();
        }
        void wait(){
            m_lock.wait();
        }
        ~AutoLock(){
            m_lock.unlock();
        }

    private:
        Lock &m_lock;
    };

}


#endif //ANDROID_MAUTOLOCK_H
