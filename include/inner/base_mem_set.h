/*
 * * file name: base_mem_set.h
 * * description:
 * *     所有set 或者map类型的容器类的基础实现，用哈希桶实现，在少量数据的时候退化
 * *     在数据量少的话退化成数组
 * * author: snow
 * * create time:2018  7 19
 * */

#ifndef _BASE_MEM_SET_H_
#define _BASE_MEM_SET_H_

#include <iterator>
#include <utility>
#include "../utils/traits_utils.h"
#include "head.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemSet
{
    using IntType = typename FixIntType<MAX_SIZE>::IntType;
    using ValueType = T;

    class Iterator
    {
        friend struct BaseMemSet;
        const BaseMemSet* m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemSet* set_, IntType index_) : m_set(set_), m_index(index_) {}

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;

        Iterator() = default;
        const T& operator*() const;
        T& operator*();
        const T* operator->() const;
        T* operator->();
        bool operator==(const Iterator& right_) const;
        bool operator!=(const Iterator& right_) const;
        Iterator& operator++();
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
    std::pair<Iterator, bool> insert(const T& value_);
    std::pair<IntType, bool> insert2(const T& value_);
    /// 找到节点
    const Iterator find(const T& value_) const;
    Iterator find(const T& value_);
    IntType find_index(const T& value_) const;
    /// 是否存在
    bool exist(const T& value_) const;
    /// 删除一个，根据迭代器
    IntType erase(const Iterator& it_);
    /// 删除一个，根据值
    IntType erase(const T& value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

    // 这两个函数加得很无奈，需要通过index来构造迭代器，但是又不想把接口暴露出来
    // 结果导致了需要通过BaseMemSet来做，有点蛋疼
    const T& deref(IntType index_) const;
    T& deref(IntType index_);

private:
    static IntType get_bucket_index(const T& value_);
    IntType find_first_used_bucket() const;
    IntType find_index(IntType bucket_index_, const T& value_) const;
    IntType insert(IntType bucket_index_, const T& value_);

    static constexpr size_t fix_bucket_size()
    {
        return (sizeof(T) > 4 && MAX_SIZE <= 40) || (sizeof(T) <= 4 && MAX_SIZE <= 50) ? 1
                                                                                       : NearByPrime<MAX_SIZE>::PRIME;
    }

    template <size_t SIZE_OF_BUCKETS>
    static IntType get_bucket_index_impl(const T& value_, SizeIdentity<SIZE_OF_BUCKETS>)
    {
        HASH hash_fun;
        return hash_fun(value_) % SIZE_OF_BUCKETS;
    }

    static IntType get_bucket_index_impl(const T& value_, SizeIdentity<1>) { return 0; }

private:
    T m_value[MAX_SIZE];
    /// 存储链表下标，每一个和value数组一一对应，为了字节对齐
    IntType m_next[MAX_SIZE] = {0};
    /// 使用了多少个节点
    IntType m_used = 0;
    // 使用的节点下标，m_next的下标，加入这个是为了clear的时候不用做多余的操作
    IntType m_raw_used = 0;
    /// 空闲链头个节点，m_next的下标，从1开始，0 表示没有
    IntType m_free_index = 0;
    // 找一个比max_size小素数会好一点
    static constexpr size_t BUCKETS_SIZE = fix_bucket_size();
    IntType m_buckets[BUCKETS_SIZE] = {0};
};

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::get_bucket_index(
    const T& value_)
{
    return get_bucket_index_impl(value_, SizeIdentity<BUCKETS_SIZE>());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::clear()
{
    memset(m_next, 0, sizeof(m_next));
    m_used = 0;
    m_raw_used = 0;
    m_free_index = 0;
    memset(m_buckets, 0, sizeof(m_buckets));
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::empty() const
{
    return m_used == 0;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::full() const
{
    return m_used == MAX_SIZE;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::size() const
{
    return m_used;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::capacity() const
{
    return MAX_SIZE;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool>
BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(const T& value_)
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

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType, bool>
BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert2(const T& value_)
{
    if (full())
        return std::make_pair(0, false);

    // 需要看看有没有存在
    IntType bucket_index = BaseMemSet::get_bucket_index(value_);
    IntType index = find_index(bucket_index, value_);
    if (index != 0)
        return std::make_pair(index, false);
    else
        return std::make_pair(insert(bucket_index, value_), true);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(
    typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType bucket_index_, const T& value_)
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

    // 挂到桶链上，如果m_next的值是LAST_INDEX，则表示是该桶链的最后一个节点
    // 其实可以把m_buckets初始化成LAST_INDEX，这样这里就不用判断了
    // 但是那样defalut的构造函数不能用了，所以还是减轻调用者的负担
    m_next[empty_index - 1] = m_buckets[bucket_index_];
    m_buckets[bucket_index_] = empty_index;

    ++m_used;

    // 一切操作完了再拷贝数据，最坏情况是某一个数据拷贝失败，但是容器的结构不会破坏
    m_value[empty_index - 1] = value_;

    return empty_index;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(
    const T& value_) const
{
    return Iterator(this, find_index(value_));
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(
    const T& value_)
{
    return Iterator(this, find_index(value_));
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find_index(
    const T& value_) const
{
    return find_index(BaseMemSet::get_bucket_index(value_), value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find_index(
    typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType bucket_index_, const T& value_) const
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

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::exist(const T& value_) const
{
    return find(value_) != end();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(
    const Iterator& it_)
{
    assert(it_.m_set == this);
    if (it_.m_index > 0)
        return erase(m_value[it_.m_index - 1]);
    return 0;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(
    const T& value_)
{
    if (m_used == 0)
        return 0;

    IntType bucket_index = BaseMemSet::get_bucket_index(value_);
    assert(bucket_index >= 0);
    assert(bucket_index < BUCKETS_SIZE);
    if (m_buckets[bucket_index] == 0)
        return 0;

    IS_EQUAL is_equal;
    IntType* pre = &(m_buckets[bucket_index]);
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

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T& BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::deref(
    typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType index_) const
{
    return m_value[index_ - 1];
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T& BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::deref(typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType index_)
{
    return m_value[index_ - 1];
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin() const
{
    return Iterator(this, find_first_used_bucket());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
{
    return Iterator(this, find_first_used_bucket());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IntType
BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find_first_used_bucket() const
{
    for (IntType i = 0; i < BUCKETS_SIZE; ++i)
    {
        if (m_buckets[i] != 0)
            return m_buckets[i];
    }
    return 0;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::end() const
{
    return Iterator(this, 0);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
{
    return Iterator(this, 0);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T& BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator*() const
{
    return m_set->m_value[m_index - 1];
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T& BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator*()
{
    return const_cast<BaseMemSet*>(m_set)->m_value[m_index - 1];
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T* BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator->() const
{
    return &(operator*());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T* BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator->()
{
    return &(operator*());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator==(const Iterator& right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator!=(const Iterator& right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator& BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::
operator++()
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

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::
operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

}  // namespace pepper

#endif
