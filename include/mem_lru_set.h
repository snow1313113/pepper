/*
 * * file name: mem_lru_set.h
 * * description: ...
 * * author: snow
 * * create time:2018  7 26
 * */

#ifndef _MEM_LRU_SET_H_
#define _MEM_LRU_SET_H_

#include "inner/base_mem_lru_set.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE, typename HASH = std::hash<T>, typename IS_EQUAL = IsEqual<T> >
class MemLRUSet : private BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>
{
public:
    using BaseType = BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>;
    using IntType = typename BaseType::IntType;
    using ValueType = typename BaseType::ValueType;
    using Iterator = typename BaseType::Iterator;
    using DisuseCallback = typename BaseType::DisuseCallback;

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
    std::pair<Iterator, bool> insert(const T& value_, bool force_ = false, DisuseCallback call_back_ = nullptr);
    /// 找到节点的迭代器
    const Iterator find(const T& value_) const;
    Iterator find(const T& value_);
    /// 是否存在，其实和find是类似的
    bool exist(const T& value_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator& it_);
    /// 删除一个，根据值
    void erase(const T& value_);
    /// 激活一下节点
    Iterator active(const T& value_);
    /// 淘汰掉几个
    size_t disuse(size_t num_, DisuseCallback call_back_ = nullptr);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();
};

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::clear()
{
    BaseType::clear();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::empty() const
{
    return BaseType::empty();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::full() const
{
    return BaseType::full();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::size() const
{
    return BaseType::size();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::capacity() const
{
    return BaseType::capacity();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
std::pair<typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator, bool>
MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::insert(const T& value_, bool force_, DisuseCallback call_back_)
{
    return BaseType::insert(value_, force_, call_back_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(
    const T& value_) const
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::find(const T& value_)
{
    return BaseType::find(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
bool MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::exist(const T& value_) const
{
    return BaseType::exist(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const Iterator& it_)
{
    BaseType::erase(it_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
void MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::erase(const T& value_)
{
    BaseType::erase(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::active(
    const T& value_)
{
    return BaseType::active(value_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
size_t MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::disuse(size_t num_, DisuseCallback call_back_)
{
    return BaseType::disuse(num_, call_back_);
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin() const
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::begin()
{
    return BaseType::begin();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
const typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end() const
{
    return BaseType::end();
}

template <typename T, size_t MAX_SIZE, typename HASH, typename IS_EQUAL>
typename MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::Iterator MemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL>::end()
{
    return BaseType::end();
}

}  // namespace pepper

#endif
