/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_COUNTDOWNLATCH_H
#define CPP_WHEEL_COUNTDOWNLATCH_H

#include "../noncopyable/noncopyable.h"
#include "../mutex/Mutex.h"
#include "../condition/Condition.h"

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    // mutex_既用于条件变量，又用于保护count_
    mutable MutexLock mutex_; // 这里之所以用mutable修饰，是因为在const成员函数中加锁或解锁会改变mutex_
    Condition condition_;
    int count_;
};

#endif //CPP_WHEEL_COUNTDOWNLATCH_H
