/*
 * * file name: mem_lru_map.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  8 17
 * */

#ifndef MEM_LRU_MAP_H
#define MEM_LRU_MAP_H

#include "inner/base_mem_lru_set.h"

namespace Pepper
{

template<typename KEY, typename VALUE, size_t MAX_SIZE>
class MemLRUMap
{
public:
    typedef std::pair<KEY, VALUE> SET_VALUE;
    typedef BaseMemLRUSet<SET_VALUE, MAX_SIZE, SimpleHash<SET_VALUE>, IsEqual<SET_VALUE>, false> BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType SetValue;
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
    std::pair<Iterator, bool> Insert(const KEY & key_, const VALUE & value_, bool force_ = false);
    /// 找到节点的迭代器
    const Iterator Find(const KEY & key_) const;
    Iterator Find(const KEY & key_);
    /// 是否存在，其实和find是类似的
    bool IsExist(const KEY & key_) const;
    /// 删除一个，根据迭代器
    void Erase(const Iterator & it_);
    /// 删除一个，根据值
    void Erase(const KEY & key_);
    /// 激活一下节点
    Iterator Active(const KEY & key_);
    /// 淘汰掉几个
    size_t Disuse(size_t num_);

    /// 迭代器
    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    BaseType m_base;
};

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::Clear()
{
    m_base.Clear();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::IsEmpty() const
{
    return m_base.IsEmpty();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::IsFull() const
{
    return m_base.IsFull();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::Size() const
{
    return m_base.Size();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::Capacity() const
{
    return m_base.Capacity();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemLRUMap<KEY, VALUE, MAX_SIZE>::Insert(const KEY & key_, const VALUE & value_, bool force_)
{
    return m_base.Insert(SET_VALUE(key_, value_), force_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::Find(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.Find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::Find(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.Find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::IsExist(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.IsExist(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::Erase(const Iterator & it_)
{
    m_base.Erase(it_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::Erase(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    m_base.Erase(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::Active(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.Active(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::Disuse(size_t num_)
{
    return m_base.Disuse(num_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::Begin() const
{
    return m_base.Begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::Begin()
{
    return m_base.Begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::End() const
{
    return m_base.End();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::End()
{
    return m_base.End();
}

}

#endif
