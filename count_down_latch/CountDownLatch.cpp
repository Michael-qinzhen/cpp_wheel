/**
 * 来源：https://github.com/chenshuo/muduo
 */

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      condition_(mutex_), // 初始化条件变量时必须要关联一个互斥锁
      count_(count)
{
}

void CountDownLatch::wait()
{
    // 这里之所以需要加锁的原因是，下面使用了条件变量
    MutexLockGuard lock(mutex_);
    while (count_ > 0)
    {
        condition_.wait();
    }
}

void CountDownLatch::countDown()
{
    // 这里加锁，既用于保护count_，也用于条件变量
    MutexLockGuard lock(mutex_);
    --count_;
    if (0 == count_)
    {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount() const
{
    MutexLockGuard lock(mutex_);

    return count_;
}
