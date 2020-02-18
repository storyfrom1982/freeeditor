//
// Created by yongge on 20-2-11.
//

#ifndef ANDROID_BUFFERPOOL_H
#define ANDROID_BUFFERPOOL_H

#include "MessageContext.h"

namespace freee{

    template<typename T>
    class SmartPtr {

    public:

        SmartPtr()
        {
            this->reference_count = new int(1);
        }

        SmartPtr(T ptr) : SmartPtr()
        {
            this->ptr = ptr;
        }

        SmartPtr(const SmartPtr<T>& sp)
        {
            this->reference_count = sp.reference_count;
            (*reference_count) ++;
        }

        virtual ~SmartPtr()
        {
            if (--(*reference_count) == 0){
//                delete ptr;
//                ptr = nullptr;
                sr_buffer_pool_put(ptr);
                delete reference_count;
                reference_count = nullptr;
            }
        }

        T get(){
            return ptr;
        }

        const SmartPtr<T>& operator =(const SmartPtr<T>& sp)
        {
            this->~SmartPtr();
            this->ptr = sp.ptr;
            this->reference_count = sp.reference_count;
            (*reference_count) ++;
            return *this;
        }

        T& operator *()
        {
            return *(ptr);
        }

        T* operator ->()
        {
            return ptr;
        }

    private:

        T ptr;
        int *reference_count;
    };

}



#endif //ANDROID_BUFFERPOOL_H
