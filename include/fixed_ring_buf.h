/*
 * * file name: fixed_ring_buf.h
 * * description: ...
 * * author: snow
 * * create time:2019  5 11
 * */

#ifndef _FIXED_RING_BUF_H_
#define _FIXED_RING_BUF_H_

#include <algorithm>
#include "inner/head.h"
#include "utils/traits_utils.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
class FixedRingBuf
{
public:
    constexpr FixedRingBuf() noexcept = default;
    /// 清空队列
    void clear();
    /// 队列是否空
    bool empty() const;
    /// 队列是否满了
    bool full() const;
    /// 当前已经用的个数
    size_t size() const;
    /// 队列最大容量
    size_t capacity() const;
    /// 队尾入队
    bool push(const T & value_, bool over_write_ = false);
    /// 队头弹出一个
    void pop();
    /// 获取队头第一个元素
    T & front(size_t index_ = 0);
    /// 获取队头第一个元素
    const T & front(size_t index_ = 0) const;
    /// 获取队尾第一个元素
    T & back();
    /// 获取队尾第一个元素
    const T & back() const;

private:
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    IntType m_start = 0;
    IntType m_end = 0;
    IntType m_used_len = 0;
    T m_buf[MAX_SIZE];
};

template<typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE>::clear()
{
    m_start = 0;
    m_end = 0;
    m_used_len = 0;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::empty() const
{
    return m_used_len == 0;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::full() const
{
    return m_used_len == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE>::size() const
{
    return m_used_len;
}

template<typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE>::capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::push(const T & value_, bool over_write_)
{
    if (over_write_)
    {
        if (full())
            pop();
    }
    else
    {
        if (full())
            return false;
    }

    m_buf[m_end] = value_;
    m_end = ((m_end + 1) % MAX_SIZE);
    ++m_used_len;

    return true;
}

template<typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE>::pop()
{
    if (empty() == false)
    {
        m_start = ((m_start + 1) % MAX_SIZE);
        --m_used_len;
    }
}

template<typename T, size_t MAX_SIZE>
T & FixedRingBuf<T, MAX_SIZE>::front(size_t index_)
{
    assert(index_ < m_used_len);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
const T & FixedRingBuf<T, MAX_SIZE>::front(size_t index_) const
{
    assert(index_ < m_used_len);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
T & FixedRingBuf<T, MAX_SIZE>::back()
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
const T & FixedRingBuf<T, MAX_SIZE>::back() const
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}
 
}

#endif
