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
    std::pair<Iterator, bool> insert(const KEY & key_, const VALUE & value_, bool force_ = false);
    /// 找到节点的迭代器
    const Iterator find(const KEY & key_) const;
    Iterator find(const KEY & key_);
    /// 是否存在，其实和find是类似的
    bool exist(const KEY & key_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const KEY & key_);
    /// 激活一下节点
    Iterator active(const KEY & key_);
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

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::clear()
{
    m_base.clear();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::empty() const
{
    return m_base.empty();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::full() const
{
    return m_base.full();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::size() const
{
    return m_base.size();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::capacity() const
{
    return m_base.capacity();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemLRUMap<KEY, VALUE, MAX_SIZE>::insert(const KEY & key_, const VALUE & value_, bool force_)
{
    return m_base.insert(SET_VALUE(key_, value_), force_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::exist(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.exist(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::erase(const Iterator & it_)
{
    m_base.erase(it_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::erase(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    m_base.erase(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::active(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return m_base.active(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::disuse(size_t num_)
{
    return m_base.disuse(num_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::begin() const
{
    return m_base.begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::begin()
{
    return m_base.begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::end() const
{
    return m_base.end();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::end()
{
    return m_base.end();
}

}

#endif
