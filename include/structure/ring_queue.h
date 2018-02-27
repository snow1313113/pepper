/*
 * * file name: ring_queue.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  2 27
 * */

#ifndef RING_QUEUE_H
#define RING_QUEUE_H

#include "../utils/traits_utils.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
class RingQueue
{
public:
    RingQueue();
    void Clear();
    bool IsEmpty() const;
    bool IsFull() const;
    size_t Size() const;
    size_t Capacity() const;
    bool Push(const T & value_, bool over_write_ = false);
    void Pop();
    T & Front();
    const T & Front() const;
    T & Back();
    const T & Back() const;
    // todo 是否增加operatr[]接口

private:
    typedef typename FixSize<MAX_SIZE>::IntType IntType;
    IntType m_start;
    IntType m_end;
    IntType m_used_len;
    T m_buf[MAX_SIZE];
};

template<typename T, size_t MAX_SIZE>
RingQueue<T, MAX_SIZE>::RingQueue():m_start(0)
                                    ,m_end(0)
                                    ,m_used_len(0)
{
}

template<typename T, size_t MAX_SIZE>
void RingQueue<T, MAX_SIZE>::Clear()
{
    m_start = 0;
    m_end = 0;
    m_used_len = 0;
}

template<typename T, size_t MAX_SIZE>
bool RingQueue<T, MAX_SIZE>::IsEmpty() const
{
    return m_used_len == 0;
}

template<typename T, size_t MAX_SIZE>
bool RingQueue<T, MAX_SIZE>::IsFull() const
{
    return m_used_len == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
size_t RingQueue<T, MAX_SIZE>::Size() const
{
    return m_used_len;
}

template<typename T, size_t MAX_SIZE>
size_t RingQueue<T, MAX_SIZE>::Capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
bool RingQueue<T, MAX_SIZE>::Push(const T & value_, bool over_write_)
{
    if (over_write_)
    {
        if (IsFull())
            Pop();
    }
    else
    {
        if (IsFull())
            return false;
    }

    m_buf[m_end] = value_;
    m_end = ((m_end + 1) % MAX_SIZE);
    ++m_used_len;

    return true;
}

template<typename T, size_t MAX_SIZE>
void RingQueue<T, MAX_SIZE>::Pop()
{
    if (IsEmpty() == false)
    {
        m_start = ((m_start + 1) % MAX_SIZE);
        --m_used_len;
    }
}

template<typename T, size_t MAX_SIZE>
T & RingQueue<T, MAX_SIZE>::Front()
{
    return m_buf[m_start];
}

template<typename T, size_t MAX_SIZE>
const T & RingQueue<T, MAX_SIZE>::Front() const
{
    return m_buf[m_start];
}

template<typename T, size_t MAX_SIZE>
T & RingQueue<T, MAX_SIZE>::Back()
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
const T & RingQueue<T, MAX_SIZE>::Back() const
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}
 
}

#endif
