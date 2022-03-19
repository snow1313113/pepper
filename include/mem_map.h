/*
 * * file name: mem_map.h
 * * description: ...
 * * author: snow
 * * create time:2018  8 03
 * */

#ifndef _MEM_MAP_H_
#define _MEM_MAP_H_

#include "inner/base_specialization.h"
#include "inner/mem_hash_table.h"
#include "inner/policy.h"

namespace pepper
{
template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
using BaseMemMap = inner::MemHashTable<KEY, VALUE, MAX_SIZE, POLICY>;

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY = BasePolicy<KEY>>
class MemMap : private BaseMemMap<KEY, VALUE, MAX_SIZE, POLICY>
{
public:
    using BaseType = BaseMemMap<KEY, VALUE, MAX_SIZE, POLICY>;
    using T = typename BaseType::ValueType;
    using IntType = typename BaseType::IntType;
    using Iterator = typename BaseType::Iterator;

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

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
void MemMap<KEY, VALUE, MAX_SIZE, POLICY>::clear()
{
    return BaseType::clear();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemMap<KEY, VALUE, MAX_SIZE, POLICY>::empty() const
{
    return BaseType::empty();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemMap<KEY, VALUE, MAX_SIZE, POLICY>::full() const
{
    return BaseType::full();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
size_t MemMap<KEY, VALUE, MAX_SIZE, POLICY>::size() const
{
    return BaseType::size();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
size_t MemMap<KEY, VALUE, MAX_SIZE, POLICY>::capacity() const
{
    return BaseType::capacity();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
std::pair<typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator, bool> MemMap<KEY, VALUE, MAX_SIZE, POLICY>::insert(
    const KEY& key_, const VALUE& value_)
{
    return BaseType::insert({key_, value_});
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::find(
    const KEY& key_) const
{
    return BaseType::find(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::find(const KEY& key_)
{
    return BaseType::find(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
bool MemMap<KEY, VALUE, MAX_SIZE, POLICY>::exist(const KEY& key_) const
{
    return BaseType::exist(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
void MemMap<KEY, VALUE, MAX_SIZE, POLICY>::erase(const Iterator& it_)
{
    BaseType::erase(it_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
void MemMap<KEY, VALUE, MAX_SIZE, POLICY>::erase(const KEY& key_)
{
    BaseType::erase(key_);
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::begin() const
{
    return BaseType::begin();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
const typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::end() const
{
    return BaseType::end();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::begin()
{
    return BaseType::begin();
}

template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY>
typename MemMap<KEY, VALUE, MAX_SIZE, POLICY>::Iterator MemMap<KEY, VALUE, MAX_SIZE, POLICY>::end()
{
    return BaseType::end();
}

}  // namespace pepper

#endif
