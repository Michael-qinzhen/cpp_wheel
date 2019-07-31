/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef CPP_WHEEL_THREADPOOL_H
#define CPP_WHEEL_THREADPOOL_H

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <deque>

#include "../noncopyable/noncopyable.h"
#include "../mutex/Mutex.h"
#include "../condition/Condition.h"
#include "../thread/Thread.h"

class ThreadPool : noncopyable
{
public:
    typedef std::function<void ()> Task;

    explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
    ~ThreadPool();

    // Must be called before start().
    void setMaxQueueSize(int maxSize)
    {
        maxQueueSize_ = maxSize;
    }
    void setThreadInitCallback(const Task& cb)
    {
        threadInitCallback_ = cb;
    }

    void start(int numThreads);
    void stop();

    const std::string& name() const
    {
        return name_;
    }

    size_t queueSize() const;

    // Could block if maxQueueSize > 0
    // There is no move-only version of std::function in C++ as of C++14.
    // So we don't need to overload a const& and an && versions
    // as we do in (Bounded)BlockingQueue.
    void run(Task f);

private:
    bool isFull() const;
    void runInThread();
    Task take();

    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    std::string name_;
    Task threadInitCallback_;
    std::vector<std::unique_ptr<Thread>> threads_;
    std::deque<Task> queue_; // 任务队列
    size_t maxQueueSize_; // 任务队列的最大大小，如果为0的话，表示任务队列为无界队列
    bool running_; // 这里为什么不使用原子类？std::atomic<bool> running_???
};

#endif //CPP_WHEEL_THREADPOOL_H
