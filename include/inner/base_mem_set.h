/*
 * * file name: base_mem_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 19
 * */

#ifndef BASE_MEM_SET_H
#define BASE_MEM_SET_H

#include <utility>
#include "head.h"
#include "../utils/traits_utils.h"

namespace Pepper
{

template<typename T, size_t SIZE>
struct IsMinSizeMemSet
{
    enum {IS_MIN = (sizeof(T) > 4 && SIZE <= 40) || (sizeof(T) <= 4 && SIZE <= 50)};
};

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL, bool IS_MIN_SIZE>
struct BaseMemSet;

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>
{
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    typedef T ValueType;

    class Iterator
    {
        friend class BaseMemSet;
        const BaseMemSet * m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemSet * set_, IntType index_) : m_set(set_), m_index(index_){}
    public:
        Iterator() = default;
        const T & operator*() const;
        T & operator*();
        const T * operator->() const;
        T * operator->();
        bool operator==(const Iterator & right_) const;
        bool operator!=(const Iterator & right_) const;
        Iterator & operator++();
        Iterator operator++(int);
        // 不提供operator--函数了，为了省空间用了单向链表，没法做性能很好的前向迭代
    };

    /// 清空列表
    void clear();
    /// 列表是否空
    bool empty() const;
    /// 列表是否满了
    bool full() const;
    /// 当前已经用的个数
    size_t size() const;
    /// 列表最大容量
    size_t capacity() const;
    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> insert(const T & value_);
    IntType insert(IntType bucket_index_, const T & value_);
    /// 找到节点
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    IntType find_index(const T & value_) const;
    IntType find_index(IntType bucket_index_, const T & value_) const;
    /// 是否存在
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    IntType erase(const Iterator & it_);
    /// 删除一个，根据值
    IntType erase(const T & value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

    // 这两个函数加得很无奈，需要通过index来构造迭代器，但是又不想把接口暴露出来
    // 结果导致了需要通过BaseMemSet来做，有点蛋疼
    const T & deref(IntType index_) const;
    T & deref(IntType index_);

    static IntType get_bucket_index(const T & value_);

private:
    IntType find_first_used_bucket() const;

private:
    /// 使用了多少个节点
    IntType m_used = 0;
    /// 空闲链头个节点，m_next的下标，从1开始，0 表示没有
    IntType m_free_index = 0;
    // 使用的节点下标，m_next的下标，加入这个是为了clear的时候不用做多余的操作
    IntType m_raw_used = 0;
    // 找一个比max_size小素数会好一点
    static const size_t BUCKETS_SIZE = NearByPrime<MAX_SIZE>::PRIME;
    IntType m_buckets[BUCKETS_SIZE] = {0};
    /// 存储链表下标，每一个和value数组一一对应，为了字节对齐
    IntType m_next[MAX_SIZE] = {0};
    T m_value[MAX_SIZE];
};

//小数据特化类型，数据长度太小直接退化成数组
template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>
{
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    typedef T ValueType;
    typedef T* Iterator;

    /// 清空列表
    void clear();
    /// 列表是否空
    bool empty() const;
    /// 列表是否满了
    bool full() const;
    /// 当前已经用的个数
    size_t size() const;
    /// 列表最大容量
    size_t capacity() const;
    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> insert(const T & value_);
    /// 找到节点的迭代器
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 是否存在，其实和find是类似的
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    /// 使用了多少个节点
    IntType m_used = 0;
    T m_value[MAX_SIZE];
};

////////////////////////////////////////////////////////////

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::get_bucket_index(const T & value_)
{
    HASH hash;
    return hash(value_) % BUCKETS_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::clear()
{
    m_used = 0;
    m_free_index = 0;
    m_raw_used = 0;
    memset(m_buckets, 0, sizeof(m_buckets));
    memset(m_next, 0, sizeof(m_next));
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::empty() const
{
    return m_used == 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::full() const
{
    return m_used == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::size() const
{
    return m_used;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator, bool> BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::insert(const T & value_)
{
    if (full())
        return std::make_pair(end(), false);

    // 需要看看有没有存在
    IntType bucket_index = BaseMemSet::get_bucket_index(value_);
    IntType index = find_index(bucket_index, value_);
    if (index != 0)
        return std::make_pair(Iterator(this, index), false);
    else
        return std::make_pair(Iterator(this, insert(bucket_index, value_)), true);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::insert(
        typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType bucket_index_,
        const T & value_)
{
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

    // 挂到桶链上，如果m_next的值是LAST_INDEX，则表示是该桶链的最后一个节点
    // 其实可以把m_buckets初始化成LAST_INDEX，这样这里就不用判断了
    // 但是那样defalut的构造函数不能用了，所以还是减轻调用者的负担
    m_next[empty_index - 1] = m_buckets[bucket_index_];
    m_buckets[bucket_index_] = empty_index;

    ++m_used;

    return empty_index;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find(const T & value_) const
{
    return Iterator(this, find_index(value_));
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find(const T & value_)
{
    return Iterator(this, find_index(value_));
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find_index(const T & value_) const
{
    return find_index(BaseMemSet::get_bucket_index(value_), value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find_index(
        typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType bucket_index_,
        const T & value_) const
{
    assert(bucket_index_ >= 0);
    assert(bucket_index_ < BUCKETS_SIZE);
    IS_EQUAL is_equal;
    for (IntType index = m_buckets[bucket_index_]; index != 0; index = m_next[index - 1])
    {
        if (is_equal(m_value[index - 1], value_))
            return index;
    }
    return 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::exist(const T & value_) const
{
    return find(value_) != end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::erase(const Iterator & it_)
{
    assert(it_.m_set == this);
    if (it_.m_index > 0)
        return erase(m_value[it_.m_index - 1]);
    return 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::erase(const T & value_)
{
    if (m_used == 0)
        return 0;

    IntType bucket_index = BaseMemSet::get_bucket_index(value_);
    assert(bucket_index >= 0);
    assert(bucket_index < BUCKETS_SIZE);
    if (m_buckets[bucket_index] == 0)
        return 0;

    IS_EQUAL is_equal;
    IntType * pre = &(m_buckets[bucket_index]);
    for (IntType index = m_buckets[bucket_index]; index != 0; pre = &(m_next[index - 1]), index = m_next[index - 1])
    {
        if (is_equal(m_value[index - 1], value_))
        {
            assert(m_used > 0);
            *pre = m_next[index - 1];
            m_next[index - 1] = m_free_index;
            m_free_index = index;
            --m_used;
            return index;
        }
    }

    return 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T & BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::deref(
        typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType index_) const
{
    return m_value[index_ - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T & BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::deref(
        typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType index_)
{
    return m_value[index_ - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::begin() const
{
    return Iterator(this, find_first_used_bucket());
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::begin()
{
    return Iterator(this, find_first_used_bucket());
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find_first_used_bucket() const
{
    for (IntType i = 0; i < BUCKETS_SIZE; ++i)
    {
        if (m_buckets[i] != 0)
            return m_buckets[i];
    }
    return 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::end() const
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::end()
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T & BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator*() const
{
    return m_set->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T & BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator*()
{
    return const_cast<BaseMemSet*>(m_set)->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T * BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator->() const
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T * BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator->()
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator==(const Iterator & right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator!=(const Iterator & right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator & BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator++()
{
    IntType next_index = m_set->m_next[m_index - 1];
    if (next_index == 0)
    {
        // 该链上的最后一个节点，找下一个hash
        IntType next_bucket = BaseMemSet::get_bucket_index(m_set->m_value[m_index - 1]) + 1;
        while (next_bucket < BUCKETS_SIZE)
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

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}
/////////////////////////////////////////////////////////////////
template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::clear()
{
    m_used = 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::empty() const
{
    return m_used == 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::full() const
{
    return m_used == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::size() const
{
    return m_used;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator, bool> BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::insert(const T & value_)
{
    if (full())
        return std::make_pair(end(), false);

    IS_EQUAL is_equal;
    for (IntType index = 0; index < m_used; ++index)
    {
        if (is_equal(m_value[index], value_))
            return std::make_pair(&(m_value[index]), false);
    }

    m_value[m_used] = value_;
    ++m_used;
    return std::make_pair(&(m_value[m_used - 1]), true);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::find(const T & value_) const
{
    IS_EQUAL is_equal;
    for (IntType index = 0; index < m_used; ++index)
    {
        if (is_equal(m_value[index], value_))
            return &(m_value[index]);
    }
    return end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::find(const T & value_)
{
    IS_EQUAL is_equal;
    for (IntType index = 0; index < m_used; ++index)
    {
        if (is_equal(m_value[index], value_))
            return &(m_value[index]);
    }
    return end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::exist(const T & value_) const
{
    return find(value_) != end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::erase(const Iterator & it_)
{
    return erase(*it_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::erase(const T & value_)
{
    if (m_used == 0)
        return;

    IS_EQUAL is_equal;
    for (IntType index = 0; index < m_used; ++index)
    {
        if (is_equal(m_value[index], value_))
        {
            if (m_used > 1 && index < m_used - 1)
                std::swap(m_value[index], m_value[m_used - 1]);
            --m_used;
            return;
        }
    }
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::begin() const
{
    return &(m_value[0]);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::begin()
{
    return &(m_value[0]);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::end() const
{
    return &(m_value[m_used]);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true>::end()
{
    return &(m_value[m_used]);
}


}

#endif
