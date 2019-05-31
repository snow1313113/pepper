/*
 * * file name: mem_map.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  8 03
 * */

#ifndef MEM_MAP_H
#define MEM_MAP_H

#include "inner/base_specialization.h"
#include "inner/base_mem_set.h"

namespace Pepper
{

template<typename KEY, typename VALUE, size_t MAX_SIZE>
class MemMap
{
public:
    typedef std::pair<KEY, VALUE> T;
    typedef BaseMemSet<T, MAX_SIZE, std::hash<T>, IsEqual<T>, IsMinSizeMemSet<T, MAX_SIZE>::IS_MIN > BaseType;
    typedef typename BaseType::IntType IntType;
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
    std::pair<Iterator, bool> insert(const KEY & key_, const VALUE & value_);
    /// 找到节点的迭代器
    const Iterator find(const KEY & key_) const;
    Iterator find(const KEY & key_);
    /// 是否存在，其实和find是类似的
    bool exist(const KEY & key_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const KEY & key_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    BaseType m_base;
};

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::clear()
{
    return m_base.clear();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::empty() const
{
    return m_base.empty();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::full() const
{
    return m_base.full();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::size() const
{
    return m_base.size();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::capacity() const
{
    return m_base.capacity();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemMap<KEY, VALUE, MAX_SIZE>::insert(const KEY & key_, const VALUE & value_)
{
    return m_base.insert(std::make_pair(key_, value_));
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_) const
{
    T temp;
    temp.first = key_;
    return m_base.find(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_)
{
    T temp;
    temp.first = key_;
    return m_base.find(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::exist(const KEY & key_) const
{
    T temp;
    temp.first = key_;
    return m_base.exist(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::erase(const Iterator & it_)
{
    m_base.erase(it_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::erase(const KEY & key_)
{
    T temp;
    temp.first = key_;
    m_base.erase(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::begin() const
{
    return m_base.begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::end() const
{
    return m_base.end();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::begin()
{
    return m_base.begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::end()
{
    return m_base.end();
}


}


#endif
