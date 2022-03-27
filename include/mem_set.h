/*
 * * file name: mem_set.h
 * * description: ...
 * * author: snow
 * * create time:2018 7月 16
 * */

#ifndef _MEM_SET_H_
#define _MEM_SET_H_

#include "inner/base_specialization.h"
#include "inner/hash_table_policy.h"
#include "inner/mem_hash_table.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE>
using BaseMemSet = inner::MemHashTable<inner::HashTablePolicy<T, void, MAX_SIZE>>;

template <typename T, size_t MAX_SIZE = 0>
class MemSet : private BaseMemSet<T, MAX_SIZE>
{
public:
    using BaseType = BaseMemSet<T, MAX_SIZE>;
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
    std::pair<Iterator, bool> insert(const T& value_);
    /// 找到节点的迭代器
    const Iterator find(const T& value_) const;
    Iterator find(const T& value_);
    /// 是否存在，其实和find是类似的
    bool exist(const T& value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator& it_);
    /// 删除一个，根据值
    void erase(const T& value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();
};

template <typename T, size_t MAX_SIZE>
void MemSet<T, MAX_SIZE>::clear()
{
    BaseType::clear();
}

template <typename T, size_t MAX_SIZE>
bool MemSet<T, MAX_SIZE>::empty() const
{
    return BaseType::empty();
}

template <typename T, size_t MAX_SIZE>
bool MemSet<T, MAX_SIZE>::full() const
{
    return BaseType::full();
}

template <typename T, size_t MAX_SIZE>
size_t MemSet<T, MAX_SIZE>::size() const
{
    return BaseType::size();
}

template <typename T, size_t MAX_SIZE>
size_t MemSet<T, MAX_SIZE>::capacity() const
{
    return BaseType::capacity();
}

template <typename T, size_t MAX_SIZE>
std::pair<typename MemSet<T, MAX_SIZE>::Iterator, bool> MemSet<T, MAX_SIZE>::insert(const T& value_)
{
    return BaseType::insert(value_);
}

template <typename T, size_t MAX_SIZE>
const typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::find(const T& value_) const
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE>
typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::find(const T& value_)
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE>
bool MemSet<T, MAX_SIZE>::exist(const T& value_) const
{
    return BaseType::exist(value_);
}

template <typename T, size_t MAX_SIZE>
void MemSet<T, MAX_SIZE>::erase(const Iterator& it_)
{
    BaseType::erase(it_);
}

template <typename T, size_t MAX_SIZE>
void MemSet<T, MAX_SIZE>::erase(const T& value_)
{
    BaseType::erase(value_);
}

template <typename T, size_t MAX_SIZE>
const typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::begin() const
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE>
typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::begin()
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE>
const typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::end() const
{
    return BaseType::end();
}

template <typename T, size_t MAX_SIZE>
typename MemSet<T, MAX_SIZE>::Iterator MemSet<T, MAX_SIZE>::end()
{
    return BaseType::end();
}

}  // namespace pepper

#endif
