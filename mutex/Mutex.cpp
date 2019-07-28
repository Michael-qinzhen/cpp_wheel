/**
 * 来源：https://github.com/chenshuo/muduo
 */

/**
 * 注意：该cpp文件中只有测试代码，Mutex和MutexGuardLock的实现代码均位于Mutex.h文件中
 */

/* ————————————以下为测试代码———————————— */

#include <stdio.h>
#include <vector>

#include "Mutex.h"
#include "../thread/Thread.h"

#if 1
#define TEST_MUTEX_MAIN
#endif

// 互斥锁的作用域范围必须大于等于要保护的全局变量，否则就起不到保护作用
MutexLock g_mutex;

// 需要保护的全局变量
int g_count = 0;

int foo()
{
    MutexLockGuard lock(g_mutex);
    ++g_count;

    return 0;
}

#ifdef TEST_MUTEX_MAIN
int main()
{
    const int kMaxThreads = 800;

    std::vector<std::unique_ptr<Thread>> threads;
    for (int nthreads = 0; nthreads < kMaxThreads; ++nthreads)
    {
        threads.emplace_back(new Thread(foo));
        threads.back()->start();
        threads.back()->join();
    }

    assert(g_count == kMaxThreads);

    return 0;
}
#endif










