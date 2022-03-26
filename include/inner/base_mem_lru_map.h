/*
 * * file name: base_mem_lru_map.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 07
 * */

#ifndef _BASE_MEM_LRU_MAP_H_
#define _BASE_MEM_LRU_MAP_H_

#include <functional>
#include <iterator>
#include "../base_struct.h"
#include "new_hash_table.h"

namespace pepper
{
namespace inner
{
template <typename POLICY>
struct BaseMemLRUMap : public POLICY
{
public:
    using BaseType = POLICY;
    using IntType = typename BaseType::IntType;
    using KeyType = typename BaseType::KeyType;
    using ValueType = typename BaseType::NodeType;
    using DisuseCallback = std::function<bool(ValueType&)>;
    using LinkNode = typename BaseType::LinkNode;

    class Iterator
    {
        friend struct BaseMemLRUMap;
        const BaseMemLRUMap* m_set = nullptr;
        IntType m_index = 0;
        Iterator(const BaseMemLRUMap* set_, IntType index_) : m_set(set_), m_index(index_) {}

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        using pointer = ValueType*;
        using reference = ValueType&;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator() = default;
        const ValueType& operator*() const;
        ValueType& operator*();
        const ValueType* operator->() const;
        ValueType* operator->();
        bool operator==(const Iterator& right_) const;
        bool operator!=(const Iterator& right_) const;
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
    };

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
    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> insert(const ValueType& value_, bool force_ = false,
                                     const DisuseCallback& call_back_ = nullptr);
    /// 找到节点的迭代器
    const Iterator find(const KeyType& key_) const;
    Iterator find(const KeyType& key_);
    /// 是否存在
    bool exist(const KeyType& key_) const;
    /// 删除一个，根据迭代器
    void erase(const Iterator& it_);
    /// 删除一个，根据值
    void erase(const KeyType& key_);
    /// 找到激活一下节点
    Iterator active(const KeyType& key_);
    /// 淘汰掉几个
    size_t disuse(size_t num_, const DisuseCallback& call_back_ = nullptr);

    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

    using BaseType::init;
    using BaseType::need_mem_size;

private:
    const ValueType& deref(IntType index_) const;
    ValueType& deref(IntType index_);
};

//////////////////////////////////////////////////////////////////////

template <typename POLICY>
void BaseMemLRUMap<POLICY>::clear()
{
    BaseType::clear();
}

template <typename POLICY>
bool BaseMemLRUMap<POLICY>::empty() const
{
    return BaseType::empty();
}

template <typename POLICY>
bool BaseMemLRUMap<POLICY>::full() const
{
    return BaseType::full();
}

template <typename POLICY>
size_t BaseMemLRUMap<POLICY>::size() const
{
    return BaseType::size();
}

template <typename POLICY>
size_t BaseMemLRUMap<POLICY>::capacity() const
{
    return BaseType::capacity();
}

template <typename POLICY>
std::pair<typename BaseMemLRUMap<POLICY>::Iterator, bool> BaseMemLRUMap<POLICY>::insert(
    const ValueType& value_, bool force_, const DisuseCallback& call_back_)
{
    if (BaseType::full())
    {
        auto iter = find(BaseType::key_of_value(value_));
        if (iter != end())
            return std::make_pair(iter, false);

        if (!force_ || disuse(1, call_back_) == 0)
            return std::make_pair(end(), false);
    }

    // 需要看看有没有存在
    auto result_pair = BaseType::insert(value_);
    if (result_pair.second)
    {
        IntType index = result_pair.first;
        // 新插入的挂到active链头
        LinkNode& head = BaseType::active_link(0);
        BaseType::active_link(head.next).prev = index;
        BaseType::active_link(index).prev = 0;
        BaseType::active_link(index).next = head.next;
        head.next = index;
    }

    return std::make_pair(Iterator(this, result_pair.first), result_pair.second);
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::find(const KeyType& key_) const
{
    return Iterator(this, BaseType::find_index(key_));
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::find(const KeyType& key_)
{
    return Iterator(this, BaseType::find_index(key_));
}

template <typename POLICY>
bool BaseMemLRUMap<POLICY>::exist(const KeyType& key_) const
{
    return BaseType::exist(key_);
}

template <typename POLICY>
void BaseMemLRUMap<POLICY>::erase(const Iterator& it_)
{
    assert(it_.m_set == this);
    erase(*it_);
}

template <typename POLICY>
void BaseMemLRUMap<POLICY>::erase(const KeyType& key_)
{
    IntType index = BaseType::erase(key_);
    if (index > 0)
    {
        IntType next_index = BaseType::active_link(index).next;
        IntType prev_index = BaseType::active_link(index).prev;
        BaseType::active_link(prev_index).next = next_index;
        BaseType::active_link(next_index).prev = prev_index;

        BaseType::active_link(index).next = 0;
        BaseType::active_link(index).prev = 0;
    }
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::active(const KeyType& key_)
{
    IntType index = BaseType::find_index(key_);
    if (index != 0)
    {
        // 先摘除
        IntType next_index = BaseType::active_link(index).next;
        IntType prev_index = BaseType::active_link(index).prev;
        BaseType::active_link(prev_index).next = next_index;
        BaseType::active_link(next_index).prev = prev_index;

        // 插入到active链的头部
        LinkNode& head = BaseType::active_link(0);
        BaseType::active_link(head.next).prev = index;
        BaseType::active_link(index).prev = 0;
        BaseType::active_link(index).next = head.next;
        head.next = index;
    }

    return Iterator(this, index);
}

template <typename POLICY>
size_t BaseMemLRUMap<POLICY>::disuse(size_t num_, const DisuseCallback& call_back_)
{
    for (size_t i = 0; i < num_; ++i)
    {
        if (BaseType::empty())
            return i;
        if (call_back_ && !call_back_(deref(BaseType::active_link(0).prev)))
            return i;
        erase(BaseType::key_of_value(deref(BaseType::active_link(0).prev)));
    }
    return num_;
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::begin() const
{
    return Iterator(this, BaseType::active_link(0).next);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::begin()
{
    return Iterator(this, BaseType::active_link(0).next);
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::end() const
{
    return Iterator(this, 0);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::end()
{
    return Iterator(this, 0);
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::ValueType& BaseMemLRUMap<POLICY>::deref(IntType index_) const
{
    return BaseType::deref(index_);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::ValueType& BaseMemLRUMap<POLICY>::deref(IntType index_)
{
    return BaseType::deref(index_);
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::ValueType& BaseMemLRUMap<POLICY>::Iterator::operator*() const
{
    return m_set->deref(m_index);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::ValueType& BaseMemLRUMap<POLICY>::Iterator::operator*()
{
    return const_cast<BaseMemLRUMap*>(m_set)->deref(m_index);
}

template <typename POLICY>
const typename BaseMemLRUMap<POLICY>::ValueType* BaseMemLRUMap<POLICY>::Iterator::operator->() const
{
    return &(operator*());
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::ValueType* BaseMemLRUMap<POLICY>::Iterator::operator->()
{
    return &(operator*());
}

template <typename POLICY>
bool BaseMemLRUMap<POLICY>::Iterator::operator==(const Iterator& right_) const
{
    return (m_set == right_.m_set) && (m_index == right_.m_index);
}

template <typename POLICY>
bool BaseMemLRUMap<POLICY>::Iterator::operator!=(const Iterator& right_) const
{
    return (m_set != right_.m_set) || (m_index != right_.m_index);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator& BaseMemLRUMap<POLICY>::Iterator::operator++()
{
    m_index = m_set->active_link(m_index).next;
    return (*this);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator& BaseMemLRUMap<POLICY>::Iterator::operator--()
{
    m_index = m_set->active_link(m_index).prev;
    return (*this);
}

template <typename POLICY>
typename BaseMemLRUMap<POLICY>::Iterator BaseMemLRUMap<POLICY>::Iterator::operator--(int)
{
    Iterator temp = (*this);
    --(*this);
    return temp;
}

}  // namespace inner
}  // namespace pepper

#endif
