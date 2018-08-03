/*
 * * file name: mem_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018 7月 16
 * */

#ifndef MEM_SET_H
#define MEM_SET_H

#include "inner/base_mem_set.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE, typename HASH = SimpleHash<T>, typename IS_EQUAL = IsEqual<T> >
class MemSet
{
public:
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, IsMinSizeMemSet<T, MAX_SIZE>::IS_MIN > BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;

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
    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> Insert(const T & value_);
    /// 找到节点的迭代器
    const Iterator Find(const T & value_) const;
    Iterator Find(const T & value_);
    /// 是否存在，其实和find是类似的
    bool IsExist(const T & value_) const;
    /// 删除一个，根据迭代器
    void Erase(const Iterator & it_);
    /// 删除一个，根据值
    void Erase(const T & value_);
    /// 迭代器
    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    BaseType m_base;
};

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Clear()
{
    m_base.Clear();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IsEmpty() const
{
    return m_base.IsEmpty();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IsFull() const
{
    return m_base.IsFull();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Size() const
{
    return m_base.Size();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Capacity() const
{
    return m_base.Capacity();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool> MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Insert(const T & value_)
{
    return m_base.Insert(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Find(const T & value_) const
{
    return m_base.Find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Find(const T & value_)
{
    return m_base.Find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::IsExist(const T & value_) const
{
    return m_base.IsExist(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Erase(const Iterator & it_)
{
    m_base.Erase(it_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Erase(const T & value_)
{
    m_base.Erase(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Begin() const
{
    return m_base.Begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Begin()
{
    return m_base.Begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::End() const
{
    return m_base.End();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::End()
{
    return m_base.End();
}

}

#endif
