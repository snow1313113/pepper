/*
 * * file name: mem_hash_table.h
 * * description:
 * *     所有set 或者map类型的容器类的基础实现，用哈希桶实现，在少量数据的时候退化
 * *     在数据量少的话退化成数组
 * * author: snow
 * * create time:2022  2 20
 * */

#ifndef _MEM_HASH_TABLE_H_
#define _MEM_HASH_TABLE_H_

#include <iterator>
#include <utility>
#include "../base_struct.h"
#include "../utils/traits_utils.h"
#include "head.h"

namespace pepper
{
namespace inner
{
template <typename T, bool INHERIT = std::is_empty<T>::value && !std::is_final<T>::value>
struct EBOProxy
{
    T& operator*() { return m_obj; }
    T const& operator*() const { return m_obj; }

private:
    static T m_obj;
};

template <typename T>
struct EBOProxy<T, true> : public T
{
    T& operator*() { return *this; }
    T const& operator*() const { return *this; }
};

template <typename HASH, typename IS_EQUAL, bool USE_LOCAL_VAR>
struct Policy : private EBOProxy<HASH>, private EBOProxy<IS_EQUAL>
{
    using Hasher = EBOProxy<HASH>;
    using IsEqual = EBOProxy<IS_EQUAL>;

    HASH& hash() { return *static_cast<Hasher&>(*this); }
    const HASH& hash() const { return *static_cast<const Hasher&>(*this); }

    IS_EQUAL& is_equal() { return *static_cast<IsEqual&>(*this); }
    const IS_EQUAL& is_equal() const { return *static_cast<const IsEqual&>(*this); }
};

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
struct MemHashTable : public POLICY
{
    using IntType = typename FixIntType<MAX_SIZE>::IntType;
    using KeyType = KEY;
    using ValueType = std::conditional_t<std::is_same_v<VALUE, void>, KEY, Pair<KEY, VALUE>>;

    class Iterator
    {
        friend struct MemHashTable;
        const MemHashTable* m_map = nullptr;
        IntType m_index = 0;
        Iterator(const MemHashTable* map_, IntType index_) : m_map(map_), m_index(index_) {}

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        using pointer = ValueType*;
        using reference = ValueType&;
        using iterator_category = std::forward_iterator_tag;

        Iterator() = default;
        const ValueType& operator*() const;
        ValueType& operator*();
        const ValueType* operator->() const;
        ValueType* operator->();
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
    std::pair<Iterator, bool> insert(const ValueType& value_);
    std::pair<IntType, bool> insert2(const ValueType& value_);
    /// 找到节点
    const Iterator find(const KeyType& key_) const;
    Iterator find(const KeyType& key_);
    IntType find_index(const KeyType& key_) const;
    /// 是否存在
    bool exist(const KeyType& value_) const;
    /// 删除一个，根据迭代器
    IntType erase(const Iterator& it_);
    /// 删除一个，根据值
    IntType erase(const KeyType& value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    static constexpr size_t fix_bucket_size()
    {
        return (sizeof(ValueType) > 4 && MAX_SIZE <= 40) || (sizeof(ValueType) <= 4 && MAX_SIZE <= 50)
                   ? 1
                   : NearByPrime<MAX_SIZE>::PRIME;
    }

    template <size_t SIZE_OF_BUCKETS>
    IntType constexpr get_bucket_index_impl(const KeyType& value_, SizeIdentity<SIZE_OF_BUCKETS>) const
    {
        return hash()(value_) % SIZE_OF_BUCKETS;
    }

    IntType constexpr get_bucket_index_impl(const KeyType& value_, SizeIdentity<1>) const { return 0; }

    IntType get_bucket_index(const KeyType& key_) const
    {
        return get_bucket_index_impl(value_, SizeIdentity<BUCKETS_SIZE>());
    }

private:
    /// 使用了多少个节点
    IntType m_used = 0;
    // 使用的节点下标，m_next的下标，加入这个是为了clear的时候不用做多余的操作
    IntType m_raw_used = 0;
    /// 空闲链头个节点，m_next的下标，从1开始，0 表示没有
    IntType m_free_index = 0;
    // 找一个比max_size小素数会好一点
    static constexpr size_t BUCKETS_SIZE = fix_bucket_size();
    IntType m_buckets[BUCKETS_SIZE] = {0};
    /// 存储链表下标，每一个和value数组一一对应，为了字节对齐
    IntType m_next[MAX_SIZE] = {0};
    ValueType m_value[MAX_SIZE];
};

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
void MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::clear()
{
    m_used = 0;
    m_raw_used = 0;
    m_free_index = 0;
    memset(m_buckets, 0, sizeof(m_buckets));
    memset(m_next, 0, sizeof(m_next));
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::empty() const
{
    return m_used == 0 && MAX_SIZE > 0;
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::full() const
{
    return m_used == MAX_SIZE;
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
size_t MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::size() const
{
    return m_used;
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
size_t MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::capacity() const
{
    return MAX_SIZE;
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
std::pair<typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator, bool>
MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::insert(const ValueType& value_)
{
    IntType bucket_index = get_bucket_index(value_);
    IntType index = find_index(bucket_index, value_);
    if (index != 0)
        return std::make_pair(Iterator(this, index), false);
    else
    {
        if (full())
            return std::make_pair(end(), false);

        return std::make_pair(Iterator(this, insert(bucket_index, value_)), true);
    }
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
std::pair<typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType, bool>
MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::insert2(const ValueType& value_)
{
    IntType bucket_index = get_bucket_index(value_);
    IntType index = find_index(bucket_index, value_);
    if (index != 0)
        return std::make_pair(index, false);
    else
    {
        if (full())
            return std::make_pair(0, false);
        return std::make_pair(insert(bucket_index, value_), true);
    }
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::insert(
    typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType bucket_index_, const ValueType& value_)
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

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::find(
    const KeyType& value_) const
{
    return Iterator(this, find_index(value_));
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::find(
    const KeyType& value_)
{
    return Iterator(this, find_index(value_));
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::find_index(
    const KeyType& value_) const
{
    return find_index(get_bucket_index(value_), value_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::find_index(
    typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType bucket_index_, const KeyType& value_) const
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

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::exist(const KeyType& value_) const
{
    return find(value_) != end();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::erase(
    const Iterator& it_)
{
    assert(it_.m_set == this);
    if (it_.m_index > 0)
        return erase(m_value[it_.m_index - 1]);
    return 0;
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::IntType MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::erase(
    const KeyType& value_)
{
    if (m_used == 0)
        return 0;

    IntType bucket_index = get_bucket_index(value_);
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

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::begin()
    const
{
    return Iterator(this, find_first_used_bucket());
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::begin()
{
    return Iterator(this, find_first_used_bucket());
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::end()
    const
{
    return Iterator(this, 0);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>::end()
{
    return Iterator(this, 0);
}

}  // namespace inner
}  // namespace pepper

#endif
