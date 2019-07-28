/**
 * 来源：https://github.com/chenshuo/muduo
 */

#ifndef __NONCOPYABLE_H
#define __NONCOPYABLE_H

/**
 * 使用说明：使用私有继承该类
 */
class noncopyable
{
public:
    // 拷贝构造函数
    noncopyable(const noncopyable&) = delete;
    // 赋值运算符。一般赋值运算符的返回类型为noncopyable&
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif // __NONCOPYABLE_H
