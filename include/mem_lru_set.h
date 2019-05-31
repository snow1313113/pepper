/*
 * * file name: mem_lru_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 26
 * */

#ifndef MEM_LRU_SET_H
#define MEM_LRU_SET_H

#include "inner/base_mem_lru_set.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE, typename HASH = std::hash<T>, typename IS_EQUAL = IsEqual<T> >
class MemLRUSet
{
public:
    typedef BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false> BaseType;
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
    std::pair<Iterator, bool> insert(const T & value_, bool force_ = false);
    /// 找到节点的迭代器
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 是否存在，其实和find是类似的
    bool exist(const T & value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    /// 激活一下节点
    Iterator active(const T & value_);
    /// 淘汰掉几个
    size_t disuse(size_t num_);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    BaseType m_base;
};

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::clear()
{
    m_base.clear();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::empty() const
{
    return m_base.empty();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::full() const
{
    return m_base.full();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::size() const
{
    return m_base.size();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::capacity() const
{
    return m_base.capacity();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool> MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(const T & value_, bool force_)
{
    return m_base.insert(value_, force_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(const T & value_) const
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(const T & value_)
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::exist(const T & value_) const
{
    return m_base.exist(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const Iterator & it_)
{
    m_base.erase(it_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const T & value_)
{
    m_base.erase(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::active(const T & value_)
{
    return m_base.active(value_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::disuse(size_t num_)
{
    return m_base.disuse(num_);
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin() const
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end() const
{
    return m_base.end();
}

template<typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
{
    return m_base.end();
}

}

#endif
