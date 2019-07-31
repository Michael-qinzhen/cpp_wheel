cmake_minimum_required(VERSION 3.14)
project(cpp_wheel)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -g)

add_executable(cpp_wheel main.cpp
        noncopyable/noncopyable.h
        mutex/Mutex.cpp mutex/Mutex.h
        count_down_latch/CountDownLatch.cpp count_down_latch/CountDownLatch.h
        condition/Condition.cpp condition/Condition.h
        atomic/Atomic.cpp atomic/Atomic.h
        thread/Thread.cpp thread/Thread.h
        blocking_queue/BlockingQueue.cpp blocking_queue/BlockingQueue.h
        thread_pool/ThreadPool.cpp thread_pool/ThreadPool.h)

target_link_libraries(cpp_wheel pthread)
