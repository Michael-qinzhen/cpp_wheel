/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_CONDITION_H
#define CPP_WHEEL_CONDITION_H

#include "../noncopyable/noncopyable.h"
#include "../mutex/Mutex.h"

class Condition : noncopyable
{
public:
    // 在初始化条件变量的时候，必须关联一个互斥锁
    explicit Condition(MutexLock& mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_; // 注意：这里是引用，即互斥锁必须由外部传入
    pthread_cond_t pcond_;
};

#endif //CPP_WHEEL_CONDITION_H
