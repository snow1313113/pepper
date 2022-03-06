/*
 * * file name: mem_set.h
 * * description: ...
 * * author: snow
 * * create time:2018 7月 16
 * */

#ifndef _MEM_SET_H_
#define _MEM_SET_H_

#include "inner/mem_hash_table.h"
#include "inner/policy.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE, typename POLICY>
using BaseMemSet = inner::MemHashTable<T, void, MAX_SIZE, POLICY>;

template <typename T, size_t MAX_SIZE, typename POLICY = DefaultPolicy<T>>
class MemSet : private BaseMemSet<T, MAX_SIZE, POLICY>
{
public:
    using BaseType = BaseMemSet<T, MAX_SIZE, POLICY>;
    using IntType = typename BaseType::IntType;
    using ValueType = typename BaseType::ValueType;
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

template <typename T, size_t MAX_SIZE, typename POLICY>
void MemSet<T, MAX_SIZE, POLICY>::clear()
{
    BaseType::clear();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
bool MemSet<T, MAX_SIZE, POLICY>::empty() const
{
    return BaseType::empty();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
bool MemSet<T, MAX_SIZE, POLICY>::full() const
{
    return BaseType::full();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
size_t MemSet<T, MAX_SIZE, POLICY>::size() const
{
    return BaseType::size();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
size_t MemSet<T, MAX_SIZE, POLICY>::capacity() const
{
    return BaseType::capacity();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
std::pair<typename MemSet<T, MAX_SIZE, POLICY>::Iterator, bool> MemSet<T, MAX_SIZE, POLICY>::insert(const T& value_)
{
    return BaseType::insert(value_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
const typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::find(const T& value_) const
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::find(const T& value_)
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
bool MemSet<T, MAX_SIZE, POLICY>::exist(const T& value_) const
{
    return BaseType::exist(value_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
void MemSet<T, MAX_SIZE, POLICY>::erase(const Iterator& it_)
{
    BaseType::erase(it_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
void MemSet<T, MAX_SIZE, POLICY>::erase(const T& value_)
{
    BaseType::erase(value_);
}

template <typename T, size_t MAX_SIZE, typename POLICY>
const typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::begin() const
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::begin()
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
const typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::end() const
{
    return BaseType::end();
}

template <typename T, size_t MAX_SIZE, typename POLICY>
typename MemSet<T, MAX_SIZE, POLICY>::Iterator MemSet<T, MAX_SIZE, POLICY>::end()
{
    return BaseType::end();
}

}  // namespace pepper

#endif
