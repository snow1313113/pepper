/*
 * * file name: mem_list.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018 4月 20
 * */

#ifndef MEM_LIST_H
#define MEM_LIST_H

#include "inner/base_mem_list.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
class MemList
{
public:
    typedef BaseMemList<T, MAX_SIZE> BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;

public:
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
    /// 从头部插入
    Iterator push_front(const T & value_);
    /// 从尾部插入
    Iterator push_back(const T & value_);
    /// 头部弹出一个
    void pop_front();
    /// 尾部弹出一个
    void pop_back();
    /// 删除一个，根据迭代器
    void erase(const Iterator & it_);
    /// 删除一个，根据值
    void erase(const T & value_);
    void erase(IntType pos_);
    /// 查找一个节点所在的迭代器，找不到返回End
    const Iterator find(const T & value_) const;
    Iterator find(const T & value_);
    /// 通过谓词查找一个节点所在的迭代器，找不到返回End
    template<typename Predicate>
    const Iterator find_if(const Predicate & p_) const;
    template<typename Predicate>
    Iterator find_if(const Predicate & p_);

    /// 根据pos找到节点，如果pos是非法的，结果未定义
    const T & get(IntType pos_) const;

    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

private:
    BaseType m_base;
};

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::clear()
{
    m_base.clear();
}

template<typename T, size_t MAX_SIZE>
bool MemList<T, MAX_SIZE>::empty() const
{
    return m_base.empty();
}

template<typename T, size_t MAX_SIZE>
bool MemList<T, MAX_SIZE>::full() const
{
    return m_base.full();
}

template<typename T, size_t MAX_SIZE>
size_t MemList<T, MAX_SIZE>::size() const
{
    return m_base.size();
}

template<typename T, size_t MAX_SIZE>
size_t MemList<T, MAX_SIZE>::capacity() const
{
    return m_base.capacity();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator  MemList<T, MAX_SIZE>::push_front(const T & value_)
{
    return m_base.push_front(value_);
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator  MemList<T, MAX_SIZE>::push_back(const T & value_)
{
    return m_base.push_back(value_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::pop_front()
{
    m_base.pop_front();
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::pop_back()
{
    m_base.pop_back();
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::erase(const Iterator & it_)
{
    m_base.erase(it_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::erase(const T & value_)
{
    m_base.erase(value_);
}

template<typename T, size_t MAX_SIZE>
void MemList<T, MAX_SIZE>::erase(IntType pos_)
{
    m_base.erase(pos_);
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::find(const T & value_) const
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::find(const T & value_)
{
    return m_base.find(value_);
}

template<typename T, size_t MAX_SIZE>
template<typename Predicate>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::find_if(const Predicate & p_) const
{
    return m_base.find_if(p_);
}

template<typename T, size_t MAX_SIZE>
template<typename Predicate>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::find_if(const Predicate & p_)
{
    return m_base.find_if(p_);
}

template<typename T, size_t MAX_SIZE>
const T & MemList<T, MAX_SIZE>::get(IntType pos_) const
{
    return m_base.get(pos_);
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::begin() const
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE>
const typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::end() const
{
    return m_base.end();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::begin()
{
    return m_base.begin();
}

template<typename T, size_t MAX_SIZE>
typename MemList<T, MAX_SIZE>::Iterator MemList<T, MAX_SIZE>::end()
{
    return m_base.end();
}

}

#endif
