/*
 * * file name: mem_map.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  8 03
 * */

#ifndef MEM_MAP_H
#define MEM_MAP_H

#include "inner/base_mem_set.h"

namespace Pepper
{

template<typename T1, typename T2>
struct SimpleHash<std::pair<T1, T2> >
{
    size_t operator()(const std::pair<T1, T2> & t_) const
    {
        return SimpleHash<T1>()(t_.first);
    }
};

template <typename T1, typename T2>
struct IsEqual<std::pair<T1, T2> >
{
    typedef std::pair<T1, T2> T;
    bool operator()(const T & x_, const T & y_) const
    {
        return IsEqual<T1>()(x_.first, y_.first);
    }
};

template<typename KEY, typename VALUE, size_t MAX_SIZE>
class MemMap
{
public:
    typedef std::pair<KEY, VALUE> T;
    typedef BaseMemSet<T, MAX_SIZE, SimpleHash<T>, IsEqual<T>, IsMinSizeMemSet<T, MAX_SIZE>::IS_MIN > BaseType;
    typedef typename BaseType::IntType IntType;
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
    std::pair<Iterator, bool> Insert(const KEY & key_, const VALUE & value_);
    /// 找到节点的迭代器
    const Iterator Find(const KEY & key_) const;
    Iterator Find(const KEY & key_);
    /// 是否存在，其实和find是类似的
    bool IsExist(const KEY & key_) const;
    /// 删除一个，根据迭代器
    void Erase(const Iterator & it_);
    /// 删除一个，根据值
    void Erase(const KEY & key_);
    /// 迭代器
    const Iterator Begin() const;
    const Iterator End() const;
    Iterator Begin();
    Iterator End();

private:
    BaseType m_base;
};

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::Clear()
{
    return m_base.Clear();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::IsEmpty() const
{
    return m_base.IsEmpty();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::IsFull() const
{
    return m_base.IsFull();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::Size() const
{
    return m_base.Size();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
size_t MemMap<KEY, VALUE, MAX_SIZE>::Capacity() const
{
    return m_base.Capacity();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
std::pair<typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator, bool> MemMap<KEY, VALUE, MAX_SIZE>::Insert(const KEY & key_, const VALUE & value_)
{
    return m_base.Insert(std::make_pair(key_, value_));
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::Find(const KEY & key_) const
{
    T temp;
    temp.first = key_;
    return m_base.Find(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::Find(const KEY & key_)
{
    T temp;
    temp.first = key_;
    return m_base.Find(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
bool MemMap<KEY, VALUE, MAX_SIZE>::IsExist(const KEY & key_) const
{
    T temp;
    temp.first = key_;
    return m_base.IsExist(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::Erase(const Iterator & it_)
{
    m_base.Erase(it_);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
void MemMap<KEY, VALUE, MAX_SIZE>::Erase(const KEY & key_)
{
    T temp;
    temp.first = key_;
    m_base.Erase(temp);
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::Begin() const
{
    return m_base.Begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
const typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::End() const
{
    return m_base.End();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::Begin()
{
    return m_base.Begin();
}

template<typename KEY, typename VALUE, size_t MAX_SIZE>
typename MemMap<KEY, VALUE, MAX_SIZE>::Iterator MemMap<KEY, VALUE, MAX_SIZE>::End()
{
    return m_base.End();
}


}


#endif
