/*
 * * file name: base_mem_lru_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 26
 * */

#ifndef BASE_MEM_LRU_SET_H
#define BASE_MEM_LRU_SET_H

#include "../base_struct.h"
#include "base_mem_set.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL, bool IS_MIN_SIZE>
class BaseMemLRUSet;

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
class BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false> : public BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false>
{
public:
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false> BaseType;
    typedef typename BaseType::Iterator Iterator;
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    typedef T ValueType;

public:
    /// 清空列表
    void Clear();
    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> Insert(const T & value_);
    /// 激活一下节点
    Iterator Active(const T & value_);

private:
    typedef Link<IntType> LinkNode;

private:
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode m_active_link[MAX_SIZE + 1];
};

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Clear()
{
    BaseType::Clear();
    m_active_link[0].prev = 0;
    m_active_link[0].next = 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator, bool> BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Insert(const T & value_)
{
    if (BaseType::IsFull())
        return std::make_pair(BaseType::End(), false);

    // 需要看看有没有存在
    HASH hash;
    IntType bucket_index = hash(value_) % BaseType::BUCKETS_SIZE;
    IntType index = BaseType::Find(bucket_index, value_);
    if (index != 0)
        return std::make_pair(BaseType::MakeIterator(index), false);

    index = BaseType::Insert(bucket_index, value_);

    // 新插入的挂到active链头
    LinkNode & head = m_active_link[0];
    m_active_link[head.next].prev = index;
    m_active_link[index].prev = 0;
    m_active_link[index].next = head.next;
    head.next = index; 

    return std::make_pair(BaseType::MakeIterator(index), true);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Active(const T & value_)
{
    HASH hash;
    IntType bucket_index = hash(value_) % BaseType::BUCKETS_SIZE;
    IntType index = BaseType::Find(bucket_index, value_);
    if (index != 0)
    {
        // 先摘除
        IntType next_index = m_active_link[index].next;
        IntType prev_index = m_active_link[index].prev;
        m_active_link[prev_index].next = next_index;
        m_active_link[next_index].prev = prev_index;

        // 插入到active链的头部
        LinkNode & head = m_active_link[0];
        m_active_link[head.next].prev = index;
        m_active_link[index].prev = 0;
        m_active_link[index].next = head.next;
        head.next = index; 
    }

    return BaseType::MakeIterator(index);
}

}

#endif
