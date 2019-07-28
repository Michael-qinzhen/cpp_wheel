/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_MUTEX_H
#define CPP_WHEEL_MUTEX_H

#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "../noncopyable/noncopyable.h"

/**
 * Use as data member of a class eg.
 *
 * class Foo
 * {
 *  public:
 *      int size() const;
 *
 *  private:
 *      mutable MutexLock mutex_; // 互斥锁的作用域范围>=要保护的对象的作用域范围
 *      std::vector<int> data_;
 * }
 */
class MutexLock : noncopyable
{
public:
    // 构造函数：初始化互斥锁
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    // 析构函数：销毁互斥锁
    ~MutexLock()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }

private:

    pthread_mutex_t mutex_;
};

/**
 * Use as a stack varible, eg.
 *
 * int Foo::size() const
 * {
 *  MutexLockGuard lock(mutex_);
 *  return data.size();
 * }
 */
class MutexLockGuard : noncopyable
{
public:
    // 构造函数：加锁
    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }

    // 析构函数：解锁
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_; // 注意：这里是引用，即指向的是外面的互斥锁。
    // 如果是普通的成员变量，比如MutexLock mutex_则起不到保护临界区的作用了，因为此时mutex_的作用范围仅限于MutexLockGuard类中
};

#endif //CPP_WHEEL_MUTEX_H
