/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_BLOCKINGQUEUE_H
#define CPP_WHEEL_BLOCKINGQUEUE_H

#include <deque>

#include "../noncopyable/noncopyable.h"
#include "../mutex/Mutex.h"
#include "../condition/Condition.h"

// 无界阻塞队列
template <typename T>
class BlockingQueue : noncopyable
{
public:
    BlockingQueue()
        : mutex_(),
          notEmpty_(mutex_),
          queue_()
    {
    }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify(); // 注意Condition::notify()不需要互斥锁保护
    }

    void put(T&& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait(); // 注意Condition::wait()需要互斥锁保护
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();

        return std::move(front);
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);

        return queue_.size();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_; // 因为该队列是无界队列，所以只需要一个队列非空的条件变量，而不需要队列已满的条件变量
    std::deque<T> queue_;
};

#endif //CPP_WHEEL_BLOCKINGQUEUE_H
