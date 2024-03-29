/*
 * * file name: mem_map.h
 * * description: ...
 * * author: snow
 * * create time:2018  8 03
 * */

#ifndef _MEM_MAP_H_
#define _MEM_MAP_H_

#include "inner/base_specialization.h"
#include "inner/hash_table_policy.h"
#include "inner/mem_hash_table.h"

namespace pepper
{
template <typename KEY, typename VALUE, size_t MAX_SIZE>
using BaseMemMap = inner::MemHashTable<inner::HashTablePolicy<KEY, VALUE, MAX_SIZE>>;

template <typename KEY, typename VALUE, size_t MAX_SIZE = 0>
class MemMap : private BaseMemMap<KEY, VALUE, MAX_SIZE>
{
public:
    using BaseType = BaseMemMap<KEY, VALUE, MAX_SIZE>;
    using IntType = typename BaseType::IntType;
    using NodeType = typename BaseType::ValueType;
    using Iterator = typename BaseType::Iterator;

    using BaseType::init;
    using BaseType::need_mem_size;

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
    std::pair<Iterator, bool> insert(const KEY& key_, const VALUE& value_);
    /// 找到节点的迭代器
    const Iterator find(const KEY& key_) const;
    Iterator find(const KEY& key_);
    /// 是否存在，其实和find是类似的
    bool exist(const KEY& key_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator& it_);
    /// 删除一个，根据值
    void erase(const KEY& key_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();
};

template <typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::clear()
{
    return BaseType::clear();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::empty() const
{
    return BaseType::empty();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::full() const
{
    return BaseType::full();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::size() const
{
    return BaseType::size();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::capacity() const
{
    return BaseType::capacity();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemMap<KEY, VALUE, MAX_SIZE>::insert(
    const KEY& key_, const VALUE& value_)
{
    return BaseType::insert({key_, value_});
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::find(const KEY& key_) const
{
    return BaseType::find(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::find(const KEY& key_)
{
    return BaseType::find(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::exist(const KEY& key_) const
{
    return BaseType::exist(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::erase(const Iterator& it_)
{
    BaseType::erase(it_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::erase(const KEY& key_)
{
    BaseType::erase(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::begin() const
{
    return BaseType::begin();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::end() const
{
    return BaseType::end();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::begin()
{
    return BaseType::begin();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::end()
{
    return BaseType::end();
}

}  // namespace pepper

#endif
