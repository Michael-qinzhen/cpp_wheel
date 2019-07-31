/**
 * 来源：https://github.com/chenshuo/muduo
 */

#include "ThreadPool.h"

ThreadPool::ThreadPool(const std::string& nameArg)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(nameArg),
      maxQueueSize_(0),
      running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        char id[32];
        snprintf(id, sizeof id, "%d", i + 1);
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
        threads_[i]->start();
    }
    if (0 == numThreads && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false; // 注意：running_的赋值也在互斥锁保护的范围内，目的是为了让其他线程能够立刻看到running_的变化？？？
        // 将阻塞在notEmpty_.wait()上的工作线程唤醒，下次执行while (running_)，就可以退出while循环了
        notEmpty_.notifyAll();
    }
    for (auto& thr : threads_)
    {
        thr->join();
    }
}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

void ThreadPool::run(Task task)
{
    // 如果线程池中线程数量为0的话，就直接执行任务函数
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        // 如果maxQueueSize_为0，即任务队列为无界队列，则工作线程永远不会阻塞在notFull.wait()上
        while (isFull())
        {
            notFull_.wait();
        }
        assert(!isFull());

        queue_.push_back(std::move(task));
        notEmpty_.notify();
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty() && running_)
    {
        notEmpty_.wait();
    }
    Task task;
    // 注意：这里会再次判断任务队列是否为空，因为当任务队列为空时，工作线程会阻塞在notEmpty_.wait()上，
    // 如果要停止线程池的话，会执行notEmpty_.notify()，而此时任务队列仍然可能为空，因此就不能执行queue_.front()等操作
    if (!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        // 因为maxQueueSize_为0表示任务队列为无界队列，所以无须通知队列不满
        if (maxQueueSize_ > 0)
        {
            notFull_.notify();
        }
    }

    return task;
}

// 如果maxQueueSize_设置为0，则该函数永远返回false，即表示队列是无界队列
bool ThreadPool::isFull() const
{
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try
    {
        if (threadInitCallback_)
        {
            threadInitCallback_();
        }
        while (running_) // 这里的running_没有用锁保护，会存在多线程间的可见性问题吗？
        {
            Task task(take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        throw;
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw;
    }
}

/* ————————————以下为测试代码———————————— */

#if 1
#define TEST_THREADPOOL_MAIN
#endif

#ifdef TEST_THREADPOOL_MAIN

void print()
{
    printf("call print()\n");
}

void printString(const std::string& str)
{
    usleep(100 * 1000);
}

void test(int maxSize)
{
    ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        pool.run(std::bind(printString, std::string(buf)));
    }

    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

int main()
{
    test(0); // 任务队列为无界队列
    test(1);
    test(5);
    test(10);
    test(50);

    return 0;
}

#endif