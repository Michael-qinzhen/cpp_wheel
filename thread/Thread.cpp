/**
 * 来源：https://github.com/chenshuo/muduo
 */

#include <sys/syscall.h>
//#include <sys/prctl.h> // TODO：在Linux中使用打开该注释

#include "Thread.h"

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch* latch_;

    ThreadData(ThreadFunc func,
               const std::string& name,
               CountDownLatch* latch)
        : func_(std::move(func)),
          name_(name),
          latch_(latch)
    {
    }

    void runInThread()
    {
        latch_->countDown();
        latch_ = NULL;

        // 设置线程名称
//        ::prctl(PR_SET_NAME, name_.c_str()); // TODO：在Linux中使用打开该注释
        try
        {
            func_();
        }
        catch (const std::exception& ex)
        {
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            throw;
        }
        catch (...)
        {
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw;
        }
    }
};

void* startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;

    return NULL;
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const std::string& n)
        : started_(false),
          joined_(false),
          pthreadId_(0),
          func_(std::move(func)), // 将左值形参func变为右值
          name_(n),
          latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &latch_);
    if (pthread_create(&pthreadId_, NULL, startThread, data))
    {
        started_ = true;
        delete data;
        perror("pthread_create");
    }
    else
    {
        latch_.wait();
    }
}

int Thread::join()
{
    assert(started_); // 为了防止用户在调用start()之前就调用了join()
    assert(!joined_); // 为了防止用户调用两次join()
    joined_ = true;

    return pthread_join(pthreadId_, NULL);
}

/* ————————————以下为测试代码———————————— */
#if 0
#define TEST_THREAD_MAIN
#endif

#ifdef TEST_THREAD_MAIN

void mysleep(int seconds)
{
    timespec t = {seconds, 0};
    nanosleep(&t, NULL);
}

void threadFunc()
{
    printf("call threadFunc()\n");
}

void threadFunc2(int x)
{
    printf("call threadFunc2()\n");
}

void threadFunc3()
{
    mysleep(1);
    printf("call threadFunc3()\n");
}

class Foo
{
public:
    explicit Foo(double x)
        : x_(x)
    {
    }

    void memberFunc()
    {
        printf("call Foo::memberFunc()\n");
    }

    void memberFunc2(const std::string& text)
    {
        printf("call Foo::memberFunc2()\n");
    }

private:
    double x_;
};

int main()
{
    Thread t1(threadFunc);
    t1.start();
    t1.join();

    Thread t2(std::bind(threadFunc2, 42), "thread for free function with argument");
    t2.start();
    t2.join();

    Foo foo(87.53);
    // (&foo)->memberFunc()
    Thread t3(std::bind(&Foo::memberFunc, &foo), "thread for member function without argument with object pointer->func()");
    t3.start();
    t3.join();

    // foo.memberFunc2()，std::bind()会复制一份foo对象
    Thread t4(std::bind(&Foo::memberFunc2, foo, std::string("Shuo Chen")));
    t4.start();
    t4.join();

    // foo.memberFunc2()，由于使用了std::ref()，std::bind()不会复制一份foo对象
    Thread t5(std::bind(&Foo::memberFunc2, std::ref(foo), std::string("Shuo Chen")));
    t5.start();
    t5.join();

    {
        Thread t6(threadFunc3);
        t6.start();
        // t6 may destruct earlier than thread creation.
        // 注意：这里并没有调用t6.join()，就可能会导致t6的析构函数比线程函数先执行，而t6的析构函数会将该线程设置为detach
    }

    mysleep(2); // 为了防止主线程先于t6中的线程函数结束

    {
        Thread t7(threadFunc3);
        t7.start();
        mysleep(2);
        // t7 destruct later than thread creation.
    }

    sleep(2);
    printf("number of created threads %d\n", Thread::numCreated());

    return 0;
}
#endif





















