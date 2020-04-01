/*
 * * file name: hash_mem_pool.h
 * * Description: 带哈希桶的定长内存池，哈希桶和FixedMemPool存在一块连续的内存上
 * *              如果是共享内存，重新attach不需要重建哈希表
 * * author: snow
 * * create time:2019  5 25
 * */

#ifndef _HASH_MEM_POOL_H_
#define _HASH_MEM_POOL_H_

#include <sys/types.h>
#include <cassert>
#include "fixed_mem_pool.h"

namespace pepper
{
template <typename KEY, typename VALUE, typename HASH = std::hash<KEY> >
class HashMemPool
{
public:
    static_assert(std::is_trivial<KEY>::value, "KEY must be trivial");
    static_assert(std::is_trivial<VALUE>::value, "VALUE must be trivial");
    // static_assert(std::is_trivially_copyable<VALUE>::value, "VALUE must be is_trivially_copyable");

    using Node = std::pair<KEY, VALUE>;
    using InnerPool = FixedMemPool<Node>;

    struct Iterator
    {
        Iterator() = default;
        const Node& operator*() const;
        Node& operator*();
        const Node* operator->() const;
        Node* operator->();
        bool operator==(const Iterator& right_) const;
        bool operator!=(const Iterator& right_) const;
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);

    private:
        friend class HashMemPool;
        typename InnerPool::Iterator m_iter;
        Iterator(const typename InnerPool::Iterator& iter) : m_iter(iter) {}
    };

public:
    // 获取内存大小
    static size_t calc_mem_size(uint32_t max_node_, uint32_t bucket_num_);

    /// 当前已经用的个数
    size_t size() const;
    /// 最大容量
    size_t capacity() const;
    /// 是否空
    bool empty() const;
    /// 是否满了
    bool full() const;
    /// 根据一段内存初始化
    bool init(void* mem_, uint32_t max_node_, uint32_t bucket_num_, uint32_t mem_size_, bool check_ = false);
    /// 清空
    void clear();
    // 插入一个节点
    std::pair<Iterator, bool> insert(const KEY& key_);
    std::pair<Iterator, bool> insert(const KEY& key_, const VALUE& value_);
    /// 查找节点
    const Iterator find(const KEY& key_) const;
    Iterator find(const KEY& key_);
    /// 获取节点，如果没有可以选择插入
    Iterator get_or_insert(const KEY& key_);
    /// 删除节点
    bool erase(const KEY& key_);
    /// 指针返回一个引用值
    size_t ref(const Node* node_) const;
    /// 引用值返回真实的指针
    const Node* deref(size_t pos_) const;
    Node* deref(size_t pos_);
    /// 获得第一个Node
    const Iterator begin() const;
    Iterator begin();
    /// 获取结尾迭代器，用作判断
    const Iterator end() const;
    Iterator end();

private:
    size_t bucket_index(const KEY& key_) const;
    size_t find_ref(const KEY& key_) const;

private:
    struct HashHeader
    {
        size_t bucket_num;
        size_t max_node;
    };

    struct HashNode : public Node
    {
        size_t next;
    };

    HashHeader* m_header = nullptr;
    size_t* m_buckets = nullptr;
    InnerPool m_pool;
};

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::size() const
{
    return m_pool.size();
}

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::capacity() const
{
    return m_pool.capacity();
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::full() const
{
    return m_pool.full();
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::empty() const
{
    return m_pool.empty();
}

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::ref(const Node* node_) const
{
    return m_pool.ptr_2_int(node_);
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Node* HashMemPool<KEY, VALUE, HASH>::deref(size_t pos_) const
{
    return m_pool.int_2_ptr(pos_);
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Node* HashMemPool<KEY, VALUE, HASH>::deref(size_t pos_)
{
    return m_pool.int_2_ptr(pos_);
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::init(void* mem_, uint32_t max_node_, uint32_t bucket_num_, uint32_t mem_size_,
                                         bool check_)
{
    assert((HashMemPool<KEY, VALUE, HASH>::calc_mem_size(max_node_, bucket_num_) <= mem_size_));

    char* p = reinterpret_cast<char*>(mem_);
    m_header = reinterpret_cast<HashHeader*>(p);
    p += sizeof(HashHeader);
    m_buckets = reinterpret_cast<size_t*>(p);
    p += sizeof(m_buckets[0]) * bucket_num_;

    if (check_)
    {
        if (m_header->bucket_num != bucket_num_ || m_header->max_node != max_node_)
            return false;

        size_t mem_pool_size = InnerPool::calc_need_size(max_node_, sizeof(HashNode));
        if (!m_pool.init(p, mem_pool_size, max_node_, sizeof(HashNode), check_))
            return false;
    }
    else
    {
        size_t mem_pool_size = InnerPool::calc_need_size(max_node_, sizeof(HashNode));
        if (!m_pool.init(p, mem_pool_size, max_node_, sizeof(HashNode), check_))
            return false;

        m_header->bucket_num = bucket_num_;
        m_header->max_node = max_node_;
        memset(m_buckets, 0, sizeof(m_buckets[0]) * bucket_num_);
    }

    return true;
}

template <typename KEY, typename VALUE, typename HASH>
void HashMemPool<KEY, VALUE, HASH>::clear()
{
    if (m_header)
    {
        init(m_header, m_header->max_node, m_header->bucket_num,
             HashMemPool<KEY, VALUE, HASH>::calc_mem_size(m_header->max_node, m_header->bucket_num), false);
    }
}

template <typename KEY, typename VALUE, typename HASH>
std::pair<typename HashMemPool<KEY, VALUE, HASH>::Iterator, bool> HashMemPool<KEY, VALUE, HASH>::insert(const KEY& key_)
{
    if (full())
        return std::make_pair(end(), false);

    size_t node_ref = find_ref(key_);
    if (node_ref != 0)
        return std::make_pair(Iterator(m_pool.int_2_iter(node_ref)), false);

    auto node = static_cast<HashNode*>(m_pool.alloc());
    if (!node)
        return std::make_pair(end(), false);

    node->first = key_;
    // 挂到链上
    size_t index = bucket_index(key_);
    node->next = m_buckets[index];
    m_buckets[index] = m_pool.ptr_2_int(node);

    return std::make_pair(Iterator(m_pool.int_2_iter(m_buckets[index])), true);
}

template <typename KEY, typename VALUE, typename HASH>
std::pair<typename HashMemPool<KEY, VALUE, HASH>::Iterator, bool> HashMemPool<KEY, VALUE, HASH>::insert(
    const KEY& key_, const VALUE& value_)
{
    auto result_pair = insert(key_);
    if (result_pair.second)
        memcpy(&(result_pair.first->second), &value_, sizeof(VALUE));
    return result_pair;
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::find(const KEY& key_) const
{
    return Iterator(m_pool.int_2_iter(find_ref(key_)));
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::find(const KEY& key_)
{
    return Iterator(m_pool.int_2_iter(find_ref(key_)));
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::get_or_insert(const KEY& key_)
{
    size_t ref = find_ref(key_);
    if (ref != 0)
        return Iterator(m_pool.int_2_iter(ref));
    else
    {
        auto result_pair = insert(key_);
        return result_pair.first;
    }
}

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::bucket_index(const KEY& key_) const
{
    HASH hash_fun;
    return hash_fun(key_) % m_header->bucket_num;
}

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::find_ref(const KEY& key_) const
{
    size_t index = bucket_index(key_);
    for (size_t ref = m_buckets[index]; ref != 0;)
    {
        auto node = static_cast<const HashNode*>(m_pool.int_2_ptr(ref));
        if (node->first == key_)
            return ref;
        ref = node->next;
    }
    return 0;
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::erase(const KEY& key_)
{
    size_t index = bucket_index(key_);
    size_t ref = m_buckets[index];
    size_t* pre = &(m_buckets[index]);
    while (ref != 0)
    {
        auto node = static_cast<HashNode*>(m_pool.int_2_ptr(ref));
        if (node->first == key_)
        {
            (*pre) = node->next;
            m_pool.free(node);
            return true;
        }
        ref = node->next;
        pre = &(node->next);
    }
    return false;
}

template <typename KEY, typename VALUE, typename HASH>
size_t HashMemPool<KEY, VALUE, HASH>::calc_mem_size(uint32_t max_node_, uint32_t bucket_num_)
{
    return sizeof(HashHeader) + sizeof(m_buckets[0]) * bucket_num_ +
           InnerPool::calc_need_size(max_node_, sizeof(HashNode));
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::begin() const
{
    return Iterator(m_pool.begin());
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::begin()
{
    return Iterator(m_pool.begin());
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::end() const
{
    return Iterator(m_pool.end());
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::end()
{
    return Iterator(m_pool.end());
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Node& HashMemPool<KEY, VALUE, HASH>::Iterator::operator*() const
{
    return (*m_iter);
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Node& HashMemPool<KEY, VALUE, HASH>::Iterator::operator*()
{
    return (*m_iter);
}

template <typename KEY, typename VALUE, typename HASH>
const typename HashMemPool<KEY, VALUE, HASH>::Node* HashMemPool<KEY, VALUE, HASH>::Iterator::operator->() const
{
    return &(operator*());
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Node* HashMemPool<KEY, VALUE, HASH>::Iterator::operator->()
{
    return &(operator*());
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::Iterator::operator==(const Iterator& right_) const
{
    return m_iter == right_.m_iter;
}

template <typename KEY, typename VALUE, typename HASH>
bool HashMemPool<KEY, VALUE, HASH>::Iterator::operator!=(const Iterator& right_) const
{
    return m_iter != right_.m_iter;
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator& HashMemPool<KEY, VALUE, HASH>::Iterator::operator++()
{
    ++m_iter;
    return (*this);
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator& HashMemPool<KEY, VALUE, HASH>::Iterator::operator--()
{
    --m_iter;
    return (*this);
}

template <typename KEY, typename VALUE, typename HASH>
typename HashMemPool<KEY, VALUE, HASH>::Iterator HashMemPool<KEY, VALUE, HASH>::Iterator::operator--(int)
{
    Iterator temp = (*this);
    --(*this);
    return temp;
}

}  // namespace pepper

#endif
