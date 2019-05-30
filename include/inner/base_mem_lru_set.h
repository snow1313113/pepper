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
struct BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>
{
public:
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, false> BaseType;
    // 不能直接用basetype的inttype，因为我们的大小是不一样的
    typedef typename FixIntType<MAX_SIZE + 1>::IntType IntType;
    typedef T ValueType;

    class Iterator
    {
        friend class BaseMemLRUSet;
        const BaseMemLRUSet * m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemLRUSet * set_, IntType index_) : m_set(set_), m_index(index_){}
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
    std::pair<Iterator, bool> insert(const T & value_, bool force_);
    /// 找到节点的迭代器
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 是否存在
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    /// 找到激活一下节点
    Iterator active(const T & value_);
    /// 淘汰掉几个
    size_t disuse(size_t num_);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    const T & deref(IntType index_) const;
    T & deref(IntType index_);

private:
    typedef Link<IntType> LinkNode;
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode m_active_link[MAX_SIZE + 1];
    BaseType m_base;
};

// todo 还没实现
template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, true>
{
public:
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, true> BaseType;
    // 不能直接用basetype的inttype，因为我们的大小是不一样的
    typedef typename FixIntType<MAX_SIZE + 1>::IntType IntType;
    typedef T ValueType;

    class Iterator
    {
        friend class BaseMemLRUSet;
        const BaseMemLRUSet * m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemLRUSet * set_, IntType index_) : m_set(set_), m_index(index_){}
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
    std::pair<Iterator, bool> insert(const T & value_, bool force_);
    /// 找到节点的迭代器
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 是否存在
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    /// 找到激活一下节点
    Iterator active(const T & value_);
    /// 淘汰掉几个
    size_t disuse(size_t num_);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    const T & deref(IntType index_) const;
    T & deref(IntType index_);

private:
    typedef Link<IntType> LinkNode;
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode m_active_link[MAX_SIZE + 1];
    BaseType m_base;
};

//////////////////////////////////////////////////////////////////////

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::clear()
{
    m_base.clear();
    m_active_link[0].prev = 0;
    m_active_link[0].next = 0;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::empty() const
{
    return m_base.empty();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::full() const
{
    return m_base.full();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::size() const
{
    return m_base.size();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::capacity() const
{
    return m_base.capacity();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator, bool> BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::insert(const T & value_, bool force_)
{
    if (m_base.full())
    {
        if (!force_ || disuse(1) == 0)
            return std::make_pair(end(), false);
    }

    // 需要看看有没有存在
    IntType bucket_index = BaseType::get_bucket_index(value_);
    IntType index = m_base.find_index(bucket_index, value_);
    if (index != 0)
        return std::make_pair(Iterator(this, index), false);

    index = m_base.insert(bucket_index, value_);

    // 新插入的挂到active链头
    LinkNode & head = m_active_link[0];
    m_active_link[head.next].prev = index;
    m_active_link[index].prev = 0;
    m_active_link[index].next = head.next;
    head.next = index; 

    return std::make_pair(Iterator(this, index), true);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find(const T & value_) const
{
    return Iterator(this, m_base.find_index(value_));
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::find(const T & value_)
{
    return Iterator(this, m_base.find_index(value_));
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::exist(const T & value_) const
{
    return m_base.exist(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::erase(const Iterator & it_)
{
    assert(it_.m_set == this);
    erase(*it_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::erase(const T & value_)
{
    IntType index = m_base.erase(value_);
    if (index > 0)
    {
        IntType next_index = m_active_link[index].next;
        IntType prev_index = m_active_link[index].prev;
        m_active_link[prev_index].next = next_index;
        m_active_link[next_index].prev = prev_index;

        m_active_link[index].next = 0;
        m_active_link[index].prev = 0;
    }
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::active(const T & value_)
{
    IntType index = m_base.find_index(value_);
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

    return Iterator(this, index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::disuse(size_t num_)
{
    for (size_t i = 0; i < num_; ++i)
    {
        if (m_base.empty())
            return i;
        erase(deref(m_active_link[0].prev));
    }
    return num_;
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::begin() const
{
    return Iterator(this, m_active_link[0].next);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::begin()
{
    return Iterator(this, m_active_link[0].next);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::end() const
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::end()
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T & BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::deref(IntType index_) const
{
    return m_base.deref(index_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T & BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::deref(IntType index_)
{
    return m_base.deref(index_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T & BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator*() const
{
    return m_set->deref(m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T & BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator*()
{
    return const_cast<BaseMemLRUSet*>(m_set)->deref(m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T * BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator->() const
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator==(const Iterator & right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator!=(const Iterator & right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator & BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator++()
{
    m_index = m_set->m_active_link[m_index].next;
    return (*this);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

}

#endif
