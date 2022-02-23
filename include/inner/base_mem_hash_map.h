/*
 * * file name: base_mem_hash_map.h
 * * description:
 * *     所有set 或者map类型的容器类的基础实现，用哈希桶实现，在少量数据的时候退化
 * *     在数据量少的话退化成数组
 * * author: snow
 * * create time:2022  2 20
 * */

#ifndef _BASE_MEM_HASH_MAP_H_
#define _BASE_MEM_HASH_MAP_H_

#include <iterator>
#include <utility>
#include "../utils/traits_utils.h"
#include "head.h"

namespace pepper
{
namespace inner
{
template <typename T, bool Inherit = std::is_empty<T>::value && !std::is_final<T>::value>
struct EBOProxy
{
    T m_obj;
    T& operator*() { return value_; }
    T const& operator*() const { return value_; }
};

template <typename T>
struct EBOProxy : public T
{
    T& operator*() { return *this; }
    T const& operator*() const { return *this; }
};

template <typename HASH, typename IS_EQUAL>
struct Policy : private EBOProxy<HASH>, private EBOProxy<IS_EQUAL>
{
    using Hasher = EBOProxy<HASH>;
    using IsEqual = EBOProxy<IS_EQUAL>;

    HASH& hasher() { return *static_cast<Hasher&>(*this); }
    const HASH& hasher() const { return *static_cast<const Hasher&>(*this); }

    IS_EQUAL& is_equal() { return *static_cast<IsEqual&>(*this); }
    const IS_EQUAL& is_equal() const { return *static_cast<const IsEqual&>(*this); }
};

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemHashMap : public Policy<HASH, IS_EQUAL>
{
    using IntType = typename FixIntType<MAX_SIZE>::IntType;
    // todo may be only KEY
    using ValueType = std::pair<KEY, VALUE>;

    class Iterator
    {
        friend struct BaseMemHashMap;
        const BaseMemHashMap* m_map = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemHashMap* map_, IntType index_) : m_map(map_), m_index(index_) {}

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
}  // namespace inner
}  // namespace pepper

#endif
