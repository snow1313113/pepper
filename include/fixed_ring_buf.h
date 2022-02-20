/*
 * * file name: fixed_ring_buf.h
 * * description: ...
 * * author: snow
 * * create time:2019  5 11
 * */

#ifndef _FIXED_RING_BUF_H_
#define _FIXED_RING_BUF_H_

#include "inner/buf_data.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE = 0>
class FixedRingBuf : public inner::FixedRingBufData<T, MAX_SIZE>
{
public:
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
    bool push(const T &value_, bool over_write_ = false);
    /// 队头弹出一个
    void pop();
    /// 获取队头第一个元素
    T &front(size_t index_ = 0);
    /// 获取队头第一个元素
    const T &front(size_t index_ = 0) const;
    /// 获取队尾第一个元素
    T &back(size_t index_ = 0);
    /// 获取队尾第一个元素
    const T &back(size_t index_ = 0) const;

private:
    using Data = inner::FixedRingBufData<T, MAX_SIZE>;
    using IntType = typename Data::IntType;
};

template <typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE>::clear()
{
    Data::set_start(0);
    Data::set_end(0);
    Data::set_used_num(0);
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::empty() const
{
    return Data::get_used_num() == 0 && 0 < Data::get_max_num();
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::full() const
{
    return Data::get_used_num() >= Data::get_max_num();
}

template <typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE>::size() const
{
    return Data::get_used_num();
}

template <typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE>::capacity() const
{
    return Data::get_max_num();
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE>::push(const T &value_, bool over_write_)
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

    Data::m_buf[Data::get_end()] = value_;
    assert(Data::get_max_num() > 0);
    Data::set_end((Data::get_end() + 1) % Data::get_max_num());
    Data::incr_used_num();
    return true;
}

template <typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE>::pop()
{
    if (empty() == false)
    {
        assert(Data::get_max_num() > 0);
        Data::set_start((Data::get_start() + 1) % Data::get_max_num());
        Data::decr_used_num();
    }
}

template <typename T, size_t MAX_SIZE>
T &FixedRingBuf<T, MAX_SIZE>::front(size_t index_)
{
    assert(index_ < Data::get_used_num());
    assert(Data::get_max_num() > 0);
    return Data::m_buf[(Data::get_start() + index_) % Data::get_max_num()];
}

template <typename T, size_t MAX_SIZE>
const T &FixedRingBuf<T, MAX_SIZE>::front(size_t index_) const
{
    return const_cast<FixedRingBuf<T, MAX_SIZE> *>(this)->front(index_);
}

template <typename T, size_t MAX_SIZE>
T &FixedRingBuf<T, MAX_SIZE>::back(size_t index_)
{
    assert(index_ < Data::get_used_num());
    assert(Data::get_max_num() > 0);
    return Data::m_buf[(Data::get_end() + Data::get_max_num() - 1 - index_) % Data::get_max_num()];
}

template <typename T, size_t MAX_SIZE>
const T &FixedRingBuf<T, MAX_SIZE>::back(size_t index_) const
{
    return const_cast<FixedRingBuf<T, MAX_SIZE> *>(this)->back(index_);
}

}  // namespace pepper

#endif
