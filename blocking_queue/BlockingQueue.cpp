//
// Created by ling jiang on 2019-07-28.
//

/**
 * 注意：该cpp文件中只有测试代码，BlockingQueue的实现代码均位于BlockingQueue.h文件中
 */

/* ————————————以下为测试代码———————————— */

#if 0
#define TEST_BLOCKINGQUEUE_MAIN
#endif

#ifdef TEST_BLOCKINGQUEUE_MAIN

#include <vector>

#include "BlockingQueue.h"
#include "../count_down_latch/CountDownLatch.h"
#include "../thread/Thread.h"

class Test
{
public:
    Test(int numThreads)
        : latch_(numThreads)
    {
        // 创建numThreads个工作线程，阻塞在queue_.take()上
        for (int i = 0; i < numThreads; ++i)
        {
            char name[32];
            snprintf(name, sizeof(name), "work thread %d", i);
            threads_.emplace_back(new Thread(std::bind(&Test::threadFunc, this), std::string(name)));
        }

        for (auto& thr : threads_)
        {
            thr->start();
        }
    }

    // 等待所有工作线程都启动之后(即均阻塞在queue_.take()上)，向队列中添加times个元素供工作线程消费
    void run(int times)
    {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i = 0; i < times; ++i)
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "hello %d", i);
            queue_.put(buf);
            printf("put data = %s, size = %zd\n", buf, queue_.size());
        }
    }

    void joinAll()
    {
        // 用于停止工作线程，因为当队列为空时，所有的工作线程都会阻塞的queue_.take()上
        for (size_t i = 0; i < threads_.size(); ++i)
        {
            queue_.put("stop");
        }

        for (auto& thr : threads_)
        {
            thr->join();
        }
    }

private:
    void threadFunc()
    {
        latch_.countDown();
        bool running = true;
        while (running)
        {
            std::string d(queue_.take());
            printf("get data = %s, size = %zd\n", d.c_str(), queue_.size());
            running = (d != "stop");
        }
    }

    BlockingQueue<std::string> queue_;
    CountDownLatch latch_;
    std::vector<std::unique_ptr<Thread>> threads_;
};

void testMove()
{
    BlockingQueue<std::unique_ptr<int>> queue;
    queue.put(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x = queue.take();
    printf("took %d\n", *x);
    *x = 123;
    queue.put(std::move(x));
    std::unique_ptr<int> y = queue.take();
    printf("took %d\n", *y);
}

int main()
{
    Test t(5);
    t.run(100);
    t.joinAll();

    testMove();

    return 0;
}

#endif
