/**
 * 来源：https://github.com/chenshuo/muduo
 */
#ifndef CPP_WHEEL_ATOMIC_H
#define CPP_WHEEL_ATOMIC_H

#include <cstdint>
#include "../noncopyable/noncopyable.h"

template <typename T>
class AtomicIntegerT : noncopyable
{
public:
    AtomicIntegerT()
        : value_(0)
    {
    }

    T get()
    {
        /**
         * T oldValue = value_;
         *
         * if (0 == value_)
         * {
         *  value_ = 0;
         * }
         *
         * return oldValue;
         */
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    // 返回(value_ + x)前的值
    T getAndAdd(T x)
    {
        /**
         * T oldValue = value;
         *
         * value_ += x;
         *
         * return oldValue;
         */
        return __sync_fetch_and_add(&value_, x);
    }

    // 返回(value_ + x)后的值
    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        decrementAndGet();
    }

    T getAndSet(T newValue)
    {
        /**
         * T oldValue = value_;
         *
         * value_ = newValue;
         *
         * return oldValue;
         */
        return __sync_lock_test_and_set(&value_, newValue);
    }

private:
    volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

#endif //CPP_WHEEL_ATOMIC_H
