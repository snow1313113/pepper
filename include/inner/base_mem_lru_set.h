/*
 * * file name: base_mem_lru_set.h
 * * description: ...
 * * author: snow
 * * create time:2018  7 26
 * */

#ifndef BASE_MEM_LRU_SET_H
#define BASE_MEM_LRU_SET_H

#include <iterator>
#include "../base_struct.h"
#include "base_mem_set.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
struct BaseMemLRUSet
{
public:
    using BaseType = BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL>;
    // 不能直接用basetype的inttype，因为我们的大小是不一样的
    using IntType = typename FixIntType<MAX_SIZE + 1>::IntType;
    using ValueType = T;
    using DisuseCallback = std::function<void(ValueType&)>;

    class Iterator
    {
        friend struct BaseMemLRUSet;
        const BaseMemLRUSet* m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemLRUSet* set_, IntType index_) : m_set(set_), m_index(index_) {}

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
    std::pair<Iterator, bool> insert(const T& value_, bool force_, DisuseCallback call_back_ = nullptr);
    /// 找到节点的迭代器
    const Iterator find(const T& value_) const;
    Iterator find(const T& value_);
    /// 是否存在
    bool exist(const T& value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator& it_);
    /// 删除一个，根据值
    void erase(const T& value_);
    /// 找到激活一下节点
    Iterator active(const T& value_);
    /// 淘汰掉几个
    size_t disuse(size_t num_, DisuseCallback call_back_ = nullptr);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    const T& deref(IntType index_) const;
    T& deref(IntType index_);

private:
    using LinkNode = Link<IntType>;
    /// 第一个节点作为flag 活跃双向链表，最近被访问的放在最前面，和每一个value数组一一对应
    LinkNode m_active_link[MAX_SIZE + 1];
    BaseType m_base;
};

//////////////////////////////////////////////////////////////////////

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::clear()
{
    m_base.clear();
    m_active_link[0].prev = 0;
    m_active_link[0].next = 0;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::empty() const
{
    return m_base.empty();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::full() const
{
    return m_base.full();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::size() const
{
    return m_base.size();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::capacity() const
{
    return m_base.capacity();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool>
BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(const T& value_, bool force_, DisuseCallback call_back_)
{
    if (m_base.full())
    {
        if (!force_ || disuse(1, call_back_) == 0)
            return std::make_pair(end(), false);
    }

    // 需要看看有没有存在
    auto result_pair = m_base.insert2(value_);
    if (result_pair.second)
    {
        IntType index = result_pair.first;
        // 新插入的挂到active链头
        LinkNode& head = m_active_link[0];
        m_active_link[head.next].prev = index;
        m_active_link[index].prev = 0;
        m_active_link[index].next = head.next;
        head.next = index;
    }
    return std::make_pair(Iterator(this, result_pair.first), result_pair.second);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(
    const T& value_) const
{
    return Iterator(this, m_base.find_index(value_));
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(
    const T& value_)
{
    return Iterator(this, m_base.find_index(value_));
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::exist(const T& value_) const
{
    return m_base.exist(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const Iterator& it_)
{
    assert(it_.m_set == this);
    erase(*it_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const T& value_)
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

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::active(
    const T& value_)
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
        LinkNode& head = m_active_link[0];
        m_active_link[head.next].prev = index;
        m_active_link[index].prev = 0;
        m_active_link[index].next = head.next;
        head.next = index;
    }

    return Iterator(this, index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::disuse(size_t num_, DisuseCallback call_back_)
{
    for (size_t i = 0; i < num_; ++i)
    {
        if (m_base.empty())
            return i;
        if (call_back_)
            call_back_(deref(m_active_link[0].prev));
        erase(deref(m_active_link[0].prev));
    }
    return num_;
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
    const
{
    return Iterator(this, m_active_link[0].next);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
{
    return Iterator(this, m_active_link[0].next);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
    const
{
    return Iterator(this, 0);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
{
    return Iterator(this, 0);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T& BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::deref(IntType index_) const
{
    return m_base.deref(index_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T& BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::deref(IntType index_)
{
    return m_base.deref(index_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T& BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator*() const
{
    return m_set->deref(m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T& BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator*()
{
    return const_cast<BaseMemLRUSet*>(m_set)->deref(m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const T* BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator->() const
{
    return &(operator*());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
T* BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator->()
{
    return &(operator*());
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator==(const Iterator& right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::operator!=(const Iterator& right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator& BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::
operator++()
{
    m_index = m_set->m_active_link[m_index].next;
    return (*this);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator::
operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

}  // namespace pepper

#endif
