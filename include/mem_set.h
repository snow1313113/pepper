/*
 * * file name: mem_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018 7月 16
 * */

#ifndef MEM_SET_H
#define MEM_SET_H

#include "head.h"
#include "utils/traits_utils.h"

template<typename T, size_t MAX_SIZE, typename HASH = SimpleHash<T>>
class MemSet
{
public:
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    typedef T ValueType;

    class Iterator
    {
        friend class MemSet;
        const MemSet * m_set;
        IntType m_index;
        Iterator(const MemSet * set_, IntType index_) : m_set(set_), m_index(index_){}
    public:
        Iterator() = default;
        const T & operator*() const;
        T & operator*();
        T * operator->() const;
        bool operator==(const Iterator & right_) const;
        bool operator!=(const Iterator & right_) const;
        Iterator & operator++();
        Iterator operator++(int);
        // 不提供operator--函数了，为了省空间用了单向链表，没法做性能很好的前向迭代
    };

public:
    /// 清空列表
    void Clear();
    /// 列表是否空
    bool IsEmpty() const;
    /// 列表是否满了
    bool IsFull() const;
    /// 当前已经用的个数
    size_t Size() const;
    /// 列表最大容量
    size_t Capacity() const;
    /// 插入一个元素，如果存在则返回失败
    Iterator Insert(const T & value_);
    /// 找到节点的迭代器
    const Iterator Find(const T & value_) const;
    Iterator Find(const T & value_);
    /// 是否存在，其实和find是类似的
    bool IsExist(const T & value_) const;
    /// 删除一个，根据迭代器
    void Erase(const Iterator & it_);
    /// 删除一个，根据值
    void Erase(const T & value_);
    /// 迭代器
    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    /// 使用了多少个节点
    IntType m_used;
    /// 空闲链头个节点，m_next的下标，从1开始，0 表示没有
    IntType m_free_index;
    // 使用的节点下标，m_next的下标，加入这个是为了clear的时候不用做多余的操作
    IntType m_raw_used;
    // todo bucket的数量不一定要max_size，因为找一个比max_size小素数会好一点
    IntType m_buckets[MAX_SIZE];
    /// 存储链表下标，每一个和value数组一一对应，为了字节对齐
    IntType m_next[MAX_SIZE];
    T m_value[MAX_SIZE];
};

template<typename T, size_t MAX_SIZE, typename HASH>
void MemSet<T, MAX_SIZE, HASH>::Clear()
{
    m_used = 0;
    m_free_index = 0;
    m_raw_used = 0;
    memset(m_buckets, 0, sizeof(m_buckets));
    memset(m_next, 0, sizeof(m_next));
}

template<typename T, size_t MAX_SIZE, typename HASH>
bool MemSet<T, MAX_SIZE, HASH>::IsEmpty() const
{
    return m_used == 0;
}

template<typename T, size_t MAX_SIZE, typename HASH>
bool MemSet<T, MAX_SIZE, HASH>::IsFull() const
{
    return m_used == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH>
size_t MemSet<T, MAX_SIZE, HASH>::Size() const
{
    return m_used;
}

template<typename T, size_t MAX_SIZE, typename HASH>
size_t MemSet<T, MAX_SIZE, HASH>::Capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Insert(const T & value_)
{
    if (IsFull())
        return End();

    IntType empty_index = 0;
    if (m_free_index == 0)
    {
        assert(m_raw_used < MAX_SIZE);
        ++m_raw_used;
        empty_index = m_raw_used;
    }
    else
    {
        empty_index = m_free_index;
        m_free_index = m_next[empty_index - 1];
    }

    assert(empty_index > 0);
    m_value[empty_index - 1] = value_;

    // todo
    // 挂到桶链上，如果m_next的值是LAST_INDEX，则表示是该桶链的最后一个节点
    // 其实可以把m_buckets初始化成LAST_INDEX，这样这里就不用判断了
    // 但是那样defalut的构造函数不能用了，所以还是让使用起来更方便吧
    HASH hash;
    IntType bucket_index = hash(value_) % MAX_SIZE;
    m_next[empty_index - 1] = m_buckets[bucket_index];
    m_buckets[bucket_index] = empty_index;

    ++m_used;

    return Iterator(this, empty_index);
}

template<typename T, size_t MAX_SIZE, typename HASH>
const typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Find(const T & value_) const
{
    HASH hash;
    IntType bucket_index = hash(value_) % MAX_SIZE;
    assert(bucket_index >= 0);
    assert(bucket_index < MAX_SIZE);
    for (IntType index = m_buckets[bucket_index]; index != 0; index = m_next[index - 1])
    {
        if (m_value[index - 1] == value_)
            return Iterator(this, index);
    }

    return End();
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Find(const T & value_)
{
    HASH hash;
    IntType bucket_index = hash(value_) % MAX_SIZE;
    assert(bucket_index >= 0);
    assert(bucket_index < MAX_SIZE);
    for (IntType index = m_buckets[bucket_index]; index != 0; index = m_next[index - 1])
    {
        if (m_value[index - 1] == value_)
            return Iterator(this, index);
    }

    return End();
}

template<typename T, size_t MAX_SIZE, typename HASH>
bool MemSet<T, MAX_SIZE, HASH>::IsExist(const T & value_) const
{
    return Find(value_) != End();
}

template<typename T, size_t MAX_SIZE, typename HASH>
void MemSet<T, MAX_SIZE, HASH>::Erase(const Iterator & it_)
{
    assert(it_.m_set == this);
    if (it_.m_index > 0)
        Erase(m_value[it_.m_index - 1]);
}

template<typename T, size_t MAX_SIZE, typename HASH>
void MemSet<T, MAX_SIZE, HASH>::Erase(const T & value_)
{
    if (m_used == 0)
        return;

    HASH hash;
    IntType bucket_index = hash(value_) % MAX_SIZE;
    assert(bucket_index >= 0);
    assert(bucket_index < MAX_SIZE);
    if (m_buckets[bucket_index] == 0)
        return;

    IntType * pre = &(m_buckets[bucket_index]);
    for (IntType index = m_buckets[bucket_index]; index != 0; pre = &(m_next[index - 1]), index = m_next[index - 1])
    {
        if (m_value[index - 1] == value_)
        {
            assert(m_used > 0);
            *pre = m_next[index - 1];
            m_next[index - 1] = m_free_index;
            m_free_index = index;
            --m_used;
            return;
        }
    }
}

template<typename T, size_t MAX_SIZE, typename HASH>
const typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Begin() const
{
    for (IntType i = 0; i < MAX_SIZE; ++i)
    {
        if (m_buckets[i] != 0)
            return Iterator(this, m_buckets[i]);
    }
    return End();
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Begin()
{
    for (IntType i = 0; i < MAX_SIZE; ++i)
    {
        if (m_buckets[i] != 0)
            return Iterator(this, m_buckets[i]);
    }
    return End();
}

template<typename T, size_t MAX_SIZE, typename HASH>
const typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::End() const
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::End()
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH>
const T & MemSet<T, MAX_SIZE, HASH>::Iterator::operator*() const
{
    return m_set->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH>
T & MemSet<T, MAX_SIZE, HASH>::Iterator::operator*()
{
    return const_cast<MemSet*>(m_set)->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH>
T * MemSet<T, MAX_SIZE, HASH>::Iterator::operator->() const
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE, typename HASH>
bool MemSet<T, MAX_SIZE, HASH>::Iterator::operator==(const Iterator & right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH>
bool MemSet<T, MAX_SIZE, HASH>::Iterator::operator!=(const Iterator & right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator & MemSet<T, MAX_SIZE, HASH>::Iterator::operator++()
{
    IntType next_index = m_set->m_next[m_index - 1];
    if (next_index == 0)
    {
        // 该链上的最后一个节点，找下一个hash
        HASH hash;
        IntType next_bucket = hash(m_set->m_value[m_index - 1]) % MAX_SIZE + 1;
        while (next_bucket < MAX_SIZE)
        {
            if (m_set->m_buckets[next_bucket] != 0)
            {
                next_index = m_set->m_buckets[next_bucket];
                break;
            }
            ++next_bucket;
        }
    }

    m_index = next_index;
    return (*this);
}

template<typename T, size_t MAX_SIZE, typename HASH>
typename MemSet<T, MAX_SIZE, HASH>::Iterator MemSet<T, MAX_SIZE, HASH>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

#endif
