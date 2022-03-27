/*
 * * file name: hash_table_policy.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 19
 * */

#ifndef _HASH_TABLE_POLICY_H_
#define _HASH_TABLE_POLICY_H_

#include "../base_struct.h"
#include "policy.h"

namespace pepper
{
namespace inner
{
template <typename KEY, typename VALUE, size_t MAX_SIZE, typename HASH = std::hash<KEY>,
          typename IS_EQUAL = IsEqual<KEY>>
struct HashTablePolicy : public BasePolicy<KEY, HASH, IS_EQUAL>
{
protected:
    using BaseType = BasePolicy<KEY, HASH, IS_EQUAL>;
    using IntType = typename FixIntType<MAX_SIZE>::IntType;
    using KeyType = KEY;
    using SecondType = VALUE;
    using NodeType = std::conditional_t<std::is_same_v<SecondType, void>, KeyType, Pair<KeyType, SecondType>>;
    using RealNodeType = std::conditional_t<std::is_trivially_copyable_v<NodeType>, NodeType, char>;

    void clear()
    {
        m_used = 0;
        m_raw_used = 0;
        m_free_index = 0;
        // todo 处理析构函数
        memset(m_buckets, 0, sizeof(m_buckets));
        memset(m_next, 0, sizeof(m_next));
    }

    IntType constexpr used() const { return m_used; }
    IntType constexpr raw_used() const { return m_raw_used; }
    IntType constexpr free_index() const { return m_free_index; }
    IntType constexpr max_num() const { return MAX_SIZE; }
    IntType constexpr buckets_num() const { return BUCKETS_SIZE; }

    IntType& buckets(size_t index_) { return m_buckets[index_]; }
    const IntType& buckets(size_t index_) const { return m_buckets[index_]; }
    IntType& next(size_t index_) { return m_next[index_]; }
    const IntType& next(size_t index_) const { return m_next[index_]; }

    inline void set_used(IntType used_) { m_used = used_; }
    inline IntType incr_used() { return ++m_used; }
    inline IntType decr_used() { return --m_used; }

    inline void set_raw_used(IntType raw_used_) { m_raw_used = raw_used_; }
    inline IntType incr_raw_used() { return ++m_raw_used; }
    inline IntType decr_raw_used() { return --m_raw_used; }

    inline void set_free_index(IntType free_index_) { m_free_index = free_index_; }

    NodeType& value(size_t index_) { return m_value[index_offset(index_)]; }
    const NodeType& value(size_t index_) const { return m_value[index_offset(index_)]; }

    template <typename T = NodeType>
    inline std::enable_if_t<std::is_trivially_copyable_v<T>> copy_value(IntType index_, const T& node_value_)
    {
        value(index_) = node_value_;
    }

    // use placement new to construct the node if is not trivially copyable
    template <typename T = NodeType>
    inline std::enable_if_t<!std::is_trivially_copyable_v<T>> copy_value(IntType index_, const T& node_value_)
    {
        new (&(value(index_))) T(node_value_);
    }

    IntType get_bucket_index(const KeyType& key_) const
    {
        return get_bucket_index_impl(key_, SizeIdentity<BUCKETS_SIZE>());
    }

private:
    static constexpr size_t fix_bucket_size()
    {
        return (sizeof(NodeType) > 4 && MAX_SIZE <= 40) || (sizeof(NodeType) <= 4 && MAX_SIZE <= 50)
                   ? 1
                   : NearByPrime<MAX_SIZE>::PRIME;
    }

    template <size_t SIZE_OF_BUCKETS>
    IntType constexpr get_bucket_index_impl(const KeyType& key_, SizeIdentity<SIZE_OF_BUCKETS>) const
    {
        return BaseType::hash()(key_) % SIZE_OF_BUCKETS;
    }

    IntType constexpr get_bucket_index_impl(const KeyType& key_, SizeIdentity<1>) const { return 0; }

    static constexpr IntType index_offset(IntType index_) { return index_ * sizeof(NodeType) / sizeof(RealNodeType); }
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
    static constexpr size_t REAL_NODE_SIZE = index_offset(MAX_SIZE);
    RealNodeType m_value[REAL_NODE_SIZE];

public:
    // not impliment
    static size_t need_mem_size(size_t max_num_, size_t buckets_num_);
    bool init(void* mem_, size_t mem_size_, size_t max_num_, size_t buckets_num_, bool check_ = false);
};

template <typename KEY, typename VALUE, typename HASH, typename IS_EQUAL>
struct HashTablePolicy<KEY, VALUE, 0, HASH, IS_EQUAL> : public BasePolicy<KEY, HASH, IS_EQUAL>
{
protected:
    using BaseType = BasePolicy<KEY, HASH, IS_EQUAL>;
    using IntType = std::size_t;
    using KeyType = KEY;
    using SecondType = VALUE;
    using NodeType = std::conditional_t<std::is_same_v<SecondType, void>, KeyType, Pair<KeyType, SecondType>>;
    using RealNodeType = std::conditional_t<std::is_trivially_copyable_v<NodeType>, NodeType, char>;

    void clear()
    {
        if (!m_head)
            return;
        m_head->m_used = 0;
        m_head->m_raw_used = 0;
        m_head->m_free_index = 0;
        // todo 处理析构函数
        memset(m_buckets, 0, sizeof(IntType) * m_head->m_buckets_num);
        memset(m_next, 0, sizeof(IntType) * m_head->m_max_num);
    }

    IntType constexpr used() const { return m_head->m_used; }
    IntType constexpr raw_used() const { return m_head->m_raw_used; }
    IntType constexpr free_index() const { return m_head->m_free_index; }
    IntType constexpr max_num() const { return m_head->m_max_num; }
    IntType constexpr buckets_num() const { return m_head->m_buckets_num; }
    IntType& buckets(size_t index_) { return m_buckets[index_]; }
    const IntType& buckets(size_t index_) const { return m_buckets[index_]; }
    IntType& next(size_t index_) { return m_next[index_]; }
    const IntType& next(size_t index_) const { return m_next[index_]; }

    inline void set_used(IntType used_) { m_head->m_used = used_; }
    inline IntType incr_used() { return ++(m_head->m_used); }
    inline IntType decr_used() { return --(m_head->m_used); }

    inline void set_raw_used(IntType raw_used_) { m_head->m_raw_used = raw_used_; }
    inline IntType incr_raw_used() { return ++(m_head->m_raw_used); }
    inline IntType decr_raw_used() { return --(m_head->m_raw_used); }

    inline void set_free_index(IntType free_index_) { m_head->m_free_index = free_index_; }

    NodeType& value(size_t index_) { return m_value[index_]; }
    const NodeType& value(size_t index_) const { return m_value[index_]; }

    template <typename T = NodeType>
    inline std::enable_if_t<std::is_trivially_copyable_v<T>> copy_value(IntType index_, const T& node_value_)
    {
        value(index_) = node_value_;
    }

    // use placement new to construct the node if is not trivially copyable
    template <typename T = NodeType>
    inline std::enable_if_t<!std::is_trivially_copyable_v<T>> copy_value(IntType index_, const T& node_value_)
    {
        new (&(value(index_))) T(node_value_);
    }

    inline IntType get_bucket_index(const KeyType& key_) const { return BaseType::hash()(key_) % buckets_num(); }

public:
    static size_t need_mem_size(size_t max_num_, size_t buckets_num_)
    {
        return sizeof(Head) + sizeof(IntType) * buckets_num_ + sizeof(IntType) * max_num_ +
               sizeof(RealNodeType) * (max_num_ * sizeof(NodeType) / sizeof(RealNodeType));
    }

    bool init(void* mem_, size_t mem_size_, size_t max_num_, size_t buckets_num_, bool check_ = false)
    {
        if (!mem_ || need_mem_size(max_num_, buckets_num_) != mem_size_)
            return false;
        auto tmp_head = reinterpret_cast<Head*>(mem_);
        if (check_)
        {
            if (tmp_head->m_mem_size != mem_size_ || tmp_head->m_max_num != max_num_ ||
                tmp_head->m_buckets_num != buckets_num_)
                return false;
        }
        else
        {
            memset(mem_, 0, mem_size_);
            tmp_head->m_max_num = max_num_;
            tmp_head->m_buckets_num = buckets_num_;
            tmp_head->m_mem_size = mem_size_;
        }
        m_head = tmp_head;
        m_buckets = reinterpret_cast<IntType*>(reinterpret_cast<uint8_t*>(mem_) + sizeof(Head));
        m_next = reinterpret_cast<IntType*>(reinterpret_cast<uint8_t*>(mem_) + sizeof(Head) +
                                            sizeof(IntType) * buckets_num_);
        m_value = reinterpret_cast<RealNodeType*>(reinterpret_cast<uint8_t*>(mem_) + sizeof(Head) +
                                                  sizeof(IntType) * buckets_num_ + sizeof(IntType) * max_num_);
        return true;
    }

private:
    static constexpr IntType index_offset(IntType index_) { return index_ * sizeof(NodeType) / sizeof(RealNodeType); }

    struct Head
    {
        /// 使用了多少个节点
        IntType m_used = 0;
        // 使用的节点下标，m_next的下标，加入这个是为了clear的时候不用做多余的操作
        IntType m_raw_used = 0;
        /// 空闲链头个节点，m_next的下标，从1开始，0 表示没有
        IntType m_free_index = 0;
        /// 最大节点数
        IntType m_max_num = 0;
        /// 最大桶数量
        IntType m_buckets_num = 0;
        /// 总内存大小
        IntType m_mem_size = 0;
    };

    Head* m_head = nullptr;
    IntType* m_buckets = nullptr;
    IntType* m_next = nullptr;
    RealNodeType* m_value = nullptr;
};

}  // namespace inner
}  // namespace pepper

#endif
