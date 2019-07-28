/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_THREAD_H
#define CPP_WHEEL_THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <string>
#include <functional>

#include "../noncopyable/noncopyable.h"
#include "../count_down_latch/CountDownLatch.h"
#include "../atomic/Atomic.h"

class Thread : noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string &name = std::string());

    ~Thread();

    void start();

    int join();

    bool started() const
    {
        return started_;
    }

    const std::string &name() const
    {
        return name_;
    }

    static int numCreated()
    {
        return numCreated_.get();
    }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    static AtomicInt32 numCreated_;
};

#endif //CPP_WHEEL_THREAD_H
