/*
 * * file name: base_mem_list.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 28
 * */

#ifndef BASE_MEM_LIST_H
#define BASE_MEM_LIST_H

#include "head.h"
#include "../utils/traits_utils.h"
#include "../base_struct.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
struct BaseMemList
{
    typedef typename FixIntType<MAX_SIZE + 1>::IntType IntType;
    typedef T ValueType;

    class Iterator
    {
        friend class BaseMemList;
        const BaseMemList * m_list;
        IntType m_index;
        Iterator(const BaseMemList * list_, IntType index_) : m_list(list_), m_index(index_){}
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
        Iterator & operator--();
        Iterator operator--(int);
        IntType ToInt() const;
    };

public:
    /// 清空列表
    void Clear();
    /// 列表是否空
    bool IsEmpty() const;
    /// 列表是否满了
    bool IsFull() const;
    /// 当前已经用的个数
    size_t Size() const;
    /// 列表最大容量
    size_t Capacity() const;
    /// 从头部插入
    Iterator PushFront(const T & value_);
    /// 从尾部插入
    Iterator PushBack(const T & value_);
    /// 头部弹出一个
    void PopFront();
    /// 尾部弹出一个
    void PopBack();
    /// 删除一个，根据迭代器
    void Erase(const Iterator & it_);
    /// 删除一个，根据值
    void Erase(const T & value_);
    void Erase(IntType pos_);
    /// 查找一个节点所在的迭代器，找不到返回End
    Iterator Find(const T & value_) const;
    /// 通过谓词查找一个节点所在的迭代器，找不到返回End
    template<typename Predicate>
    Iterator FindIf(const Predicate & p_) const;

    /// 根据pos找到节点，如果pos是非法的，结果未定义
    const T & Get(IntType pos_) const;

    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    typedef Link<IntType> LinkNode;
    IntType m_used;
    // 这个是空闲连头下标，m_link的下标
    IntType m_free_index;
    // 使用的节点下标，m_link的下标
    IntType m_raw_used;
    // 第一个节点作为flag
    LinkNode m_link[MAX_SIZE + 1];
    T m_value[MAX_SIZE];
};

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::Clear()
{
    m_used = 0;
    m_link[0].prev = 0;
    m_link[0].next = 0;
    m_free_index = 0;
    m_raw_used = 0;
}

template<typename T, size_t MAX_SIZE>
bool BaseMemList<T, MAX_SIZE>::IsEmpty() const
{
    return m_used == 0;
}

template<typename T, size_t MAX_SIZE>
bool BaseMemList<T, MAX_SIZE>::IsFull() const
{
    return m_used >= MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
size_t BaseMemList<T, MAX_SIZE>::Size() const
{
    return m_used;
}

template<typename T, size_t MAX_SIZE>
size_t BaseMemList<T, MAX_SIZE>::Capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator  BaseMemList<T, MAX_SIZE>::PushFront(const T & value_)
{
    if (IsFull())
        return End();

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
        m_free_index = m_link[m_free_index].next;
    }

    assert(empty_index > 0);
    m_value[empty_index - 1] = value_;

    LinkNode & head = m_link[0];
    m_link[head.next].prev = empty_index;
    m_link[empty_index].prev = 0;
    m_link[empty_index].next = head.next;
    head.next = empty_index; 

    ++m_used;

    return Iterator(this, empty_index);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator  BaseMemList<T, MAX_SIZE>::PushBack(const T & value_)
{
    if (IsFull())
        return End();

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
        m_free_index = m_link[m_free_index].next;
    }

    assert(empty_index > 0);
    m_value[empty_index - 1] = value_;

    LinkNode & head = m_link[0];
    m_link[head.prev].next = empty_index;
    m_link[empty_index].prev = head.prev;
    m_link[empty_index].next = 0;
    head.prev = empty_index; 

    ++m_used;

    return Iterator(this, empty_index);
}

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::PopFront()
{
    if (IsEmpty() == false)
    {
        LinkNode & head = m_link[0];
        IntType del_index = head.next;
        LinkNode & del_link = m_link[del_index];

        head.next = del_link.next;
        m_link[del_link.next].prev = 0;

        // 挂到空闲链上
        del_link.next = m_free_index;
        m_free_index = del_index;

        assert(m_used > 0);
        --m_used;
    }
}

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::PopBack()
{
    if (IsEmpty() == false)
    {
        LinkNode & head = m_link[0];
        IntType del_index = head.prev;
        LinkNode & del_link = m_link[del_index];

        head.prev = del_link.prev;
        m_link[del_link.prev].next = 0;

        // 挂到空闲链上
        del_link.next = m_free_index;
        m_free_index = del_index;

        assert(m_used > 0);
        --m_used;
    }
}

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::Erase(const Iterator & it_)
{
    assert(it_.m_list == this);
    assert(m_used > 0);
    LinkNode & del_link = m_link[it_.m_index];
    LinkNode & prev_link = m_link[del_link.prev];
    LinkNode & next_link = m_link[del_link.next];
    prev_link.next = del_link.next;
    next_link.prev = del_link.prev;

    // 挂到空闲链上
    del_link.next = m_free_index;
    m_free_index = it_.m_index;

    --m_used;
}

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::Erase(const T & value_)
{
    Iterator del = Find(value_);
    if (del != End())
        Erase(del);
}

template<typename T, size_t MAX_SIZE>
void BaseMemList<T, MAX_SIZE>::Erase(IntType pos_)
{
    Erase(Iterator(this, pos_));
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::Find(const T & value_) const
{
    for (IntType i = m_link[0].next; i != 0; i = m_link[i].next)
    {
        if (m_value[i - 1] == value_)
            return Iterator(this, i);
    }

    return End();
}

template<typename T, size_t MAX_SIZE>
template<typename Predicate>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::FindIf(const Predicate & p_) const
{
    for (IntType i = m_link[0].next; i != 0; i = m_link[i].next)
    {
        if (p_(m_value[i - 1]))
            return Iterator(this, i);
    }

    return End();
}

template<typename T, size_t MAX_SIZE>
const T & BaseMemList<T, MAX_SIZE>::Get(IntType pos_) const
{
    return m_value[pos_ - 1];
}

template<typename T, size_t MAX_SIZE>
const typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::Begin() const
{
    return Iterator(this, m_link[0].next);
}

template<typename T, size_t MAX_SIZE>
const typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::End() const
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::Begin()
{
    return Iterator(this, m_link[0].next);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::End()
{
    return Iterator(this, 0);
}

template<typename T, size_t MAX_SIZE>
const T & BaseMemList<T, MAX_SIZE>::Iterator::operator*() const
{
    return m_list->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE>
T & BaseMemList<T, MAX_SIZE>::Iterator::operator*()
{
    return const_cast<BaseMemList*>(m_list)->m_value[m_index - 1];
}

template<typename T, size_t MAX_SIZE>
const T * BaseMemList<T, MAX_SIZE>::Iterator::operator->() const
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE>
T * BaseMemList<T, MAX_SIZE>::Iterator::operator->()
{
    return &(operator*());
}

template<typename T, size_t MAX_SIZE>
bool BaseMemList<T, MAX_SIZE>::Iterator::operator==(const Iterator & right_) const
{
    return (m_list == right_.m_list) && (m_index == right_.m_index);
}

template<typename T, size_t MAX_SIZE>
bool BaseMemList<T, MAX_SIZE>::Iterator::operator!=(const Iterator & right_) const
{
    return (m_list != right_.m_list) || (m_index != right_.m_index);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator & BaseMemList<T, MAX_SIZE>::Iterator::operator++()
{
    m_index = m_list->m_link[m_index].next;
    return (*this);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator & BaseMemList<T, MAX_SIZE>::Iterator::operator--()
{
    m_index = m_list->m_link[m_index].prev;
    return (*this);
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::Iterator BaseMemList<T, MAX_SIZE>::Iterator::operator--(int)
{
    Iterator temp = (*this);
    --(*this);
    return temp;
}

template<typename T, size_t MAX_SIZE>
typename BaseMemList<T, MAX_SIZE>::IntType BaseMemList<T, MAX_SIZE>::Iterator::ToInt() const
{
    return m_index;
}

}

#endif
