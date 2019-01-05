/*
 * * file name: mem_list.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018 4月 20
 * */

#ifndef MEM_LIST_H
#define MEM_LIST_H

#include "inner/base_mem_list.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
class MemList
{
public:
    typedef BaseMemList<T, MAX_SIZE> BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;

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
    const Iterator Find(const T & value_) const;
    Iterator Find(const T & value_);
    /// 通过谓词查找一个节点所在的迭代器，找不到返回End
    template<typename Predicate>
    const Iterator FindIf(const Predicate & p_) const;
    template<typename Predicate>
    Iterator FindIf(const Predicate & p_);

    /// 根据pos找到节点，如果pos是非法的，结果未定义
    const T & Get(IntType pos_) const;

    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    BaseType m_base;
};

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::Clear()
{
    m_base.Clear();
}

template<typename T, size_t MAX_SIZE>
bool MemList<T, MAX_SIZE>::IsEmpty() const
{
    return m_base.IsEmpty();
}

template<typename T, size_t MAX_SIZE>
bool MemList<T, MAX_SIZE>::IsFull() const
{
    return m_base.IsFull();
}

template<typename T, size_t MAX_SIZE>
size_t MemList<T, MAX_SIZE>::Size() const
{
    return m_base.Size();
}

template<typename T, size_t MAX_SIZE>
size_t MemList<T, MAX_SIZE>::Capacity() const
{
    return m_base.Capacity();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator  MemList<T, MAX_SIZE>::PushFront(const T & value_)
{
    return m_base.PushFront(value_);
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator  MemList<T, MAX_SIZE>::PushBack(const T & value_)
{
    return m_base.PushBack(value_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::PopFront()
{
    m_base.PopFront();
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::PopBack()
{
    m_base.PopBack();
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::Erase(const Iterator & it_)
{
    m_base.Erase(it_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::Erase(const T & value_)
{
    m_base.Erase(value_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::Erase(IntType pos_)
{
    m_base.Erase(pos_);
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::Find(const T & value_) const
{
    return m_base.Find(value_);
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::Find(const T & value_)
{
    return m_base.Find(value_);
}

template<typename T, size_t MAX_SIZE>
template<typename Predicate>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::FindIf(const Predicate & p_) const
{
    return m_base.FindIf(p_);
}

template<typename T, size_t MAX_SIZE>
template<typename Predicate>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::FindIf(const Predicate & p_)
{
    return m_base.FindIf(p_);
}

template<typename T, size_t MAX_SIZE>
const T & MemList<T, MAX_SIZE>::Get(IntType pos_) const
{
    return m_base.Get(pos_);
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::Begin() const
{
    return m_base.Begin();
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::End() const
{
    return m_base.End();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::Begin()
{
    return m_base.Begin();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::End()
{
    return m_base.End();
}

}

#endif
