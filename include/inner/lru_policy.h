/*
 * * file name: lru_policy.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 20
 * */

#ifndef _LRU_POLICY_H_
#define _LRU_POLICY_H_

#include "../base_struct.h"
#include "hash_table_policy.h"
#include "new_hash_table.h"

namespace pepper
{
namespace inner
{
template <typename KEY, typename VALUE, size_t MAX_SIZE, typename HASH = std::hash<KEY>,
          typename IS_EQUAL = IsEqual<KEY>>
struct LRUPolicy : private exp::MemHashTable<HashTablePolicy<KEY, VALUE, MAX_SIZE, HASH, IS_EQUAL>>
{
protected:
    using TableType = exp::MemHashTable<HashTablePolicy<KEY, VALUE, MAX_SIZE, HASH, IS_EQUAL>>;
    // 要多用一个节点，所以要能存下MAX_SIZE + 1
    using IntType = typename FixIntType<MAX_SIZE + 1>::IntType;
    using KeyType = KEY;
    using SecondType = VALUE;
    using NodeType = std::conditional_t<std::is_same_v<SecondType, void>, KeyType, Pair<KeyType, SecondType>>;
    using LinkNode = Link<IntType>;

    const LinkNode& active_link(size_t index_) const { return m_active_link[index_]; }
    LinkNode& active_link(size_t index_) { return m_active_link[index_]; }

    using TableType::capacity;
    using TableType::clear;
    using TableType::deref;
    using TableType::empty;
    using TableType::erase;
    using TableType::find_index;
    using TableType::full;
    using TableType::key_of_value;
    using TableType::size;

    void clear()
    {
        TableType::clear();
        m_active_link[0].prev = 0;
        m_active_link[0].next = 0;
    }

    std::pair<IntType, bool> insert(const NodeType& value_) { return TableType::insert2(value_); }

    static constexpr size_t need_mem_size(size_t max_num_, size_t buckets_num_);
    bool init(void* mem_, size_t mem_size_, size_t max_num_, size_t buckets_num_, bool check_ = false);

private:
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode m_active_link[MAX_SIZE + 1];
};

template <typename KEY, typename VALUE, typename HASH, typename IS_EQUAL>
struct LRUPolicy<KEY, VALUE, 0, HASH, IS_EQUAL> : exp::MemHashTable<HashTablePolicy<KEY, VALUE, 0, HASH, IS_EQUAL>>
{
protected:
    using TableType = exp::MemHashTable<HashTablePolicy<KEY, VALUE, 0, HASH, IS_EQUAL>>;
    using IntType = std::size_t;
    using KeyType = KEY;
    using SecondType = VALUE;
    using NodeType = std::conditional_t<std::is_same_v<SecondType, void>, KeyType, Pair<KeyType, SecondType>>;
    using LinkNode = Link<IntType>;

    const LinkNode& active_link(size_t index_) const { return m_active_link[index_]; }
    LinkNode& active_link(size_t index_) { return m_active_link[index_]; }

    using TableType::capacity;
    using TableType::clear;
    using TableType::deref;
    using TableType::empty;
    using TableType::erase;
    using TableType::find_index;
    using TableType::full;
    using TableType::key_of_value;
    using TableType::size;

    void clear()
    {
        TableType::clear();
        if (m_active_link)
        {
            m_active_link[0].prev = 0;
            m_active_link[0].next = 0;
        }
    }

    std::pair<IntType, bool> insert(const NodeType& value_) { return TableType::insert2(value_); }

    static constexpr size_t need_mem_size(size_t max_num_, size_t buckets_num_)
    {
        return sizeof(LinkNode) * (max_num_ + 1) + TableType::need_mem_size(max_num_, buckets_num_);
    }

    bool init(void* mem_, size_t mem_size_, size_t max_num_, size_t buckets_num_, bool check_ = false)
    {
        if (!mem_ || need_mem_size(max_num_, buckets_num_) != mem_size_)
            return false;

        size_t table_mem_size = TableType::need_mem_size(max_num_, buckets_num_);
        if (!TableType::init(mem_, table_mem_size, max_num_, buckets_num_, check_))
            return false;

        m_active_link = reinterpret_cast<LinkNode*>(reinterpret_cast<uint8_t*>(mem_) + table_mem_size);
        if (!check_)
            memset(m_active_link, 0, sizeof(LinkNode) * (max_num_ + 1));

        return true;
    }

private:
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode* m_active_link = nullptr;
};

}  // namespace inner

}  // namespace pepper

#endif
