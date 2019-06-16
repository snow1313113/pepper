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

template<typename T, size_t MAX_SIZE, typename HASH = std::hash<T>, typename IS_EQUAL = IsEqual<T> >
class MemSet
{
public:
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL> BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;

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
    std::pair<Iterator, bool> insert(const T & value_);
    /// 找到节点的迭代器
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 是否存在，其实和find是类似的
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    BaseType m_base;
};

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::clear()
{
    m_base.clear();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::empty() const
{
    return m_base.empty();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::full() const
{
    return m_base.full();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::size() const
{
    return m_base.size();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::capacity() const
{
    return m_base.capacity();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool> MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(const T & value_)
{
    return m_base.insert(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(const T & value_) const
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(const T & value_)
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::exist(const T & value_) const
{
    return m_base.exist(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const Iterator & it_)
{
    m_base.erase(it_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const T & value_)
{
    m_base.erase(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin() const
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::end() const
{
    return m_base.end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
{
    return m_base.end();
}

}

#endif
