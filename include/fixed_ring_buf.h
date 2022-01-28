/*
 * * file name: fixed_ring_buf.h
 * * description: ...
 * * author: snow
 * * create time:2019  5 11
 * */

#ifndef _FIXED_RING_BUF_H_
#define _FIXED_RING_BUF_H_

#include <algorithm>
#include <type_traits>
#include "inner/head.h"
#include "utils/traits_utils.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE = 0, typename = void>
class FixedRingBuf;

template <typename T, size_t MAX_SIZE>
class FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>
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
    using IntType = typename FixIntType<MAX_SIZE>::IntType;
    IntType m_start = 0;
    IntType m_end = 0;
    IntType m_used_num = 0;
    T m_buf[MAX_SIZE];
};

/// SIZE 如果是0，则表示大小是通过init来指定
template <typename T>
class FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>
{
public:
    /// 计算需要的内存
    static size_t mem_size(size_t size_);
    /// 调用者提供队列的内存
    bool init(void *mem_, size_t mem_size_, bool check_ = false);

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
    using IntType = size_t;
    struct BuffHead
    {
        IntType m_start = 0;
        IntType m_end = 0;
        IntType m_used_num = 0;
        IntType m_max_num = 0;
    };
    BuffHead *m_head = nullptr;
    T *m_buf = nullptr;
};

////////////////////////////////////////////////////
template <typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::clear()
{
    m_start = 0;
    m_end = 0;
    m_used_num = 0;
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::empty() const
{
    return m_used_num == 0;
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::full() const
{
    return m_used_num == MAX_SIZE;
}

template <typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::size() const
{
    return m_used_num;
}

template <typename T, size_t MAX_SIZE>
size_t FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::capacity() const
{
    return MAX_SIZE;
}

template <typename T, size_t MAX_SIZE>
bool FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::push(const T &value_,
                                                                                        bool over_write_)
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
    ++m_used_num;

    return true;
}

template <typename T, size_t MAX_SIZE>
void FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::pop()
{
    if (empty() == false)
    {
        m_start = ((m_start + 1) % MAX_SIZE);
        --m_used_num;
    }
}

template <typename T, size_t MAX_SIZE>
T &FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::front(size_t index_)
{
    assert(index_ < m_used_num);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template <typename T, size_t MAX_SIZE>
const T &FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::front(size_t index_) const
{
    assert(index_ < m_used_num);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template <typename T, size_t MAX_SIZE>
T &FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::back(size_t index_)
{
    assert(index_ < m_used_num);
    return m_buf[(m_end + MAX_SIZE - 1 - index_) % MAX_SIZE];
}

template <typename T, size_t MAX_SIZE>
const T &FixedRingBuf<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>::back(size_t index_) const
{
    assert(index_ < m_used_num);
    return m_buf[(m_end + MAX_SIZE - 1 - index_) % MAX_SIZE];
}

////////////////////////////////////////////////////
template <typename T>
size_t FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::mem_size(size_t size_)
{
    return sizeof(BuffHead) + sizeof(T) * size_;
}

template <typename T>
bool FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::init(void *mem_, size_t mem_size_,
                                                                                 bool check_)
{
    if (!mem_ || mem_size_ < sizeof(BuffHead))
        return false;

    m_head = reinterpret_cast<BuffHead *>(mem_);
    if (check_)
    {
        if (m_head->m_max_num != (mem_size_ - sizeof(BuffHead)) / sizeof(T))
            return false;
    }
    else
    {
        m_head->m_start = 0;
        m_head->m_end = 0;
        m_head->m_used_num = 0;
        m_head->m_max_num = (mem_size_ - sizeof(BuffHead)) / sizeof(T);
    }
    m_buf = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(mem_) + sizeof(BuffHead));
    return true;
}

template <typename T>
void FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::clear()
{
    m_head->m_start = 0;
    m_head->m_end = 0;
    m_head->m_used_num = 0;
}

template <typename T>
bool FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::empty() const
{
    return m_head->m_used_num == 0 && m_head->m_used_num < m_head->m_max_num;
}

template <typename T>
bool FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::full() const
{
    return m_head->m_used_num == m_head->m_max_num;
}

template <typename T>
size_t FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::size() const
{
    return m_head->m_used_num;
}

template <typename T>
size_t FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::capacity() const
{
    return m_head->m_max_num;
}

template <typename T>
bool FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::push(const T &value_, bool over_write_)
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

    m_buf[m_head->m_end] = value_;
    assert(m_head->m_max_num > 0);
    m_head->m_end = ((m_head->m_end + 1) % m_head->m_max_num);
    ++(m_head->m_used_num);
    return true;
}

template <typename T>
void FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::pop()
{
    if (empty() == false)
    {
        assert(m_head->m_max_num > 0);
        m_head->m_start = ((m_head->m_start + 1) % m_head->m_max_num);
        --(m_head->m_used_num);
    }
}

template <typename T>
T &FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::front(size_t index_)
{
    assert(m_head);
    assert(index_ < m_head->m_used_num);
    assert(m_head->m_max_num > 0);
    return m_buf[(m_head->m_start + index_) % m_head->m_max_num];
}

template <typename T>
const T &FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::front(size_t index_) const
{
    assert(m_head);
    assert(index_ < m_head->m_used_num);
    assert(m_head->m_max_num > 0);
    return m_buf[(m_head->m_start + index_) % m_head->m_max_num];
}

template <typename T>
T &FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::back(size_t index_)
{
    assert(m_head);
    assert(index_ < m_head->m_used_num);
    assert(m_head->m_max_num > 0);
    return m_buf[(m_head->m_end + m_head->m_max_num - 1 - index_) % m_head->m_max_num];
}

template <typename T>
const T &FixedRingBuf<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>::back(size_t index_) const
{
    assert(m_head);
    assert(index_ < m_head->m_used_num);
    assert(m_head->m_max_num > 0);
    return m_buf[(m_head->m_end + m_head->m_max_num - 1 - index_) % m_head->m_max_num];
}

}  // namespace pepper

#endif
