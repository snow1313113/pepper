/*
 * * file name: mem_lru_map.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  8 17
 * */

#ifndef MEM_LRU_MAP_H
#define MEM_LRU_MAP_H

#include "inner/base_specialization.h"
#include "inner/base_mem_lru_set.h"

namespace Pepper
{

template<typename KEY, typename VALUE, size_t MAX_SIZE>
class MemLRUMap : private BaseMemLRUSet<std::pair<KEY, VALUE>, MAX_SIZE, std::hash<std::pair<KEY, VALUE>>, IsEqual<std::pair<KEY, VALUE>>>
{
public:
    typedef std::pair<KEY, VALUE> SET_VALUE;
    typedef BaseMemLRUSet<SET_VALUE, MAX_SIZE, std::hash<SET_VALUE>, IsEqual<SET_VALUE>> BaseType;
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
};

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::clear()
{
    BaseType::clear();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::empty() const
{
    return BaseType::empty();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::full() const
{
    return BaseType::full();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::size() const
{
    return BaseType::size();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::capacity() const
{
    return BaseType::capacity();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemLRUMap<KEY, VALUE, MAX_SIZE>::insert(const KEY & key_, const VALUE & value_, bool force_)
{
    return BaseType::insert(SET_VALUE(key_, value_), force_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return BaseType::find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::find(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return BaseType::find(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemLRUMap<KEY, VALUE, MAX_SIZE>::exist(const KEY & key_) const
{
    SET_VALUE set_value;
    set_value.first = key_;
    return BaseType::exist(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::erase(const Iterator & it_)
{
    BaseType::erase(it_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemLRUMap<KEY, VALUE, MAX_SIZE>::erase(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    BaseType::erase(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::active(const KEY & key_)
{
    SET_VALUE set_value;
    set_value.first = key_;
    return BaseType::active(set_value);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemLRUMap<KEY, VALUE, MAX_SIZE>::disuse(size_t num_)
{
    return BaseType::disuse(num_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::begin() const
{
    return BaseType::begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::begin()
{
    return BaseType::begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::end() const
{
    return BaseType::end();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemLRUMap<KEY, VALUE, MAX_SIZE>::Iterator MemLRUMap<KEY, VALUE, MAX_SIZE>::end()
{
    return BaseType::end();
}

}

#endif
