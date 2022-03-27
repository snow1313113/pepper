/*
 * * file name: mem_hash_table.h
 * * description:
 * *     所有set 或者map类型的容器类的基础实现，用哈希桶实现，在少量数据的时候退化
 * *     在数据量少的话退化成数组
 * * author: snow
 * * create time:2022  2 20
 * */

#ifndef _MEM_HASH_TABLE_H_
#define _MEM_HASH_TABLE_H_

#include <iterator>
#include <utility>
#include "../base_struct.h"
#include "../utils/traits_utils.h"
#include "head.h"

namespace pepper
{
namespace inner
{
template <typename POLICY>
struct MemHashTable : public POLICY
{
    using BaseType = POLICY;
    using IntType = typename BaseType::IntType;
    using KeyType = typename BaseType::KeyType;
    using ValueType = typename BaseType::NodeType;

    class Iterator
    {
        friend struct MemHashTable;
        const MemHashTable* m_table = nullptr;
        IntType m_index = 0;
        Iterator(const MemHashTable* table_, IntType index_) : m_table(table_), m_index(index_) {}

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;
        using pointer = ValueType*;
        using reference = ValueType&;
        using iterator_category = std::forward_iterator_tag;

        Iterator() = default;
        const ValueType& operator*() const;
        ValueType& operator*();
        const ValueType* operator->() const;
        ValueType* operator->();
        bool operator==(const Iterator& right_) const;
        bool operator!=(const Iterator& right_) const;
        Iterator& operator++();
        Iterator operator++(int);
        // 不提供operator--函数了，为了省空间用了单向链表，没法做性能很好的前向迭代
    };

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
    std::pair<Iterator, bool> insert(const ValueType& value_);
    std::pair<IntType, bool> insert2(const ValueType& value_);
    /// 找到节点
    const Iterator find(const KeyType& key_) const;
    Iterator find(const KeyType& key_);
    IntType find_index(const KeyType& key_) const;
    /// 是否存在
    bool exist(const KeyType& value_) const;
    /// 删除一个，根据迭代器
    IntType erase(const Iterator& it_);
    /// 删除一个，根据值
    IntType erase(const KeyType& value_);
    /// 迭代器
    const Iterator begin() const;
    const Iterator end() const;
    Iterator begin();
    Iterator end();

    // 这两个函数加得很无奈，需要通过index来构造迭代器，但是又不想把接口暴露出来
    // 结果导致了需要通过BaseMemSet来做，有点蛋疼
    const ValueType& deref(IntType index_) const;
    ValueType& deref(IntType index_);

    const KeyType& key_of_value(const KeyType& key_) const { return key_; }
    using SecondType = std::conditional_t<std::is_same_v<ValueType, KeyType>, bool, typename BaseType::SecondType>;
    const KeyType& key_of_value(const Pair<KeyType, SecondType>& pair_) const { return pair_.first; }

private:
    IntType find_first_used_bucket() const;
    IntType find_index_impl(IntType bucket_index_, const KeyType& value_) const;
    IntType insert(IntType bucket_index_, const ValueType& value_);
};

template <typename POLICY>
void MemHashTable<POLICY>::clear()
{
    BaseType::clear();
}

template <typename POLICY>
bool MemHashTable<POLICY>::empty() const
{
    return BaseType::used() == 0 && BaseType::max_num() > 0;
}

template <typename POLICY>
bool MemHashTable<POLICY>::full() const
{
    return BaseType::used() == BaseType::max_num();
}

template <typename POLICY>
size_t MemHashTable<POLICY>::size() const
{
    return BaseType::used();
}

template <typename POLICY>
size_t MemHashTable<POLICY>::capacity() const
{
    return BaseType::max_num();
}

template <typename POLICY>
std::pair<typename MemHashTable<POLICY>::Iterator, bool> MemHashTable<POLICY>::insert(const ValueType& value_)
{
    IntType bucket_index = BaseType::get_bucket_index(key_of_value(value_));
    IntType index = find_index_impl(bucket_index, key_of_value(value_));
    if (index != 0)
        return std::make_pair(Iterator(this, index), false);
    else
    {
        if (full())
            return std::make_pair(end(), false);

        return std::make_pair(Iterator(this, insert(bucket_index, value_)), true);
    }
}

template <typename POLICY>
std::pair<typename MemHashTable<POLICY>::IntType, bool> MemHashTable<POLICY>::insert2(const ValueType& value_)
{
    IntType bucket_index = BaseType::get_bucket_index(key_of_value(value_));
    IntType index = find_index_impl(bucket_index, key_of_value(value_));
    if (index != 0)
        return std::make_pair(index, false);
    else
    {
        if (full())
            return std::make_pair(0, false);
        return std::make_pair(insert(bucket_index, value_), true);
    }
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::insert(IntType bucket_index_, const ValueType& value_)
{
    IntType empty_index = 0;
    if (BaseType::free_index() == 0)
    {
        assert(BaseType::raw_used() < BaseType::max_num());
        empty_index = BaseType::incr_raw_used();
    }
    else
    {
        empty_index = BaseType::free_index();
        BaseType::set_free_index(BaseType::next(empty_index - 1));
    }

    assert(empty_index > 0);

    // 挂到桶链上，如果next的值是LAST_INDEX，则表示是该桶链的最后一个节点
    // 其实可以把buckets初始化成LAST_INDEX，这样这里就不用判断了
    // 但是那样defalut的构造函数不能用了，所以还是减轻调用者的负担
    BaseType::next(empty_index - 1) = BaseType::buckets(bucket_index_);
    BaseType::buckets(bucket_index_) = empty_index;

    BaseType::incr_used();

    // 一切操作完了再拷贝数据，最坏情况是某一个数据拷贝失败，但是容器的结构不会破坏
    BaseType::copy_value(empty_index - 1, value_);

    return empty_index;
}

template <typename POLICY>
const typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::find(const KeyType& value_) const
{
    return Iterator(this, find_index(value_));
}

template <typename POLICY>
typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::find(const KeyType& value_)
{
    return Iterator(this, find_index(value_));
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::find_index(const KeyType& value_) const
{
    return find_index_impl(BaseType::get_bucket_index(value_), value_);
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::find_index_impl(IntType bucket_index_,
                                                                             const KeyType& value_) const
{
    assert(bucket_index_ >= 0);
    assert(bucket_index_ < BaseType::buckets_num());
    auto&& equal = POLICY::is_equal();
    for (IntType index = BaseType::buckets(bucket_index_); index != 0; index = BaseType::next(index - 1))
    {
        if (equal(key_of_value(BaseType::value(index - 1)), value_))
            return index;
    }
    return 0;
}

template <typename POLICY>
bool MemHashTable<POLICY>::exist(const KeyType& value_) const
{
    return find(value_) != end();
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::erase(const Iterator& it_)
{
    assert(it_.m_table == this);
    if (it_.m_index > 0)
        return erase(BaseType::value(it_.m_index - 1));
    return 0;
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::erase(const KeyType& value_)
{
    if (BaseType::used() == 0)
        return 0;

    IntType bucket_index = BaseType::get_bucket_index(value_);
    assert(bucket_index >= 0);
    assert(bucket_index < BaseType::buckets_num());
    if (BaseType::buckets(bucket_index) == 0)
        return 0;

    auto&& equal = POLICY::is_equal();
    IntType* pre = &(BaseType::buckets(bucket_index));
    for (IntType index = BaseType::buckets(bucket_index); index != 0;
         pre = &(BaseType::next(index - 1)), index = BaseType::next(index - 1))
    {
        if (equal(key_of_value(BaseType::value(index - 1)), value_))
        {
            assert(BaseType::used() > 0);
            *pre = BaseType::next(index - 1);
            BaseType::next(index - 1) = BaseType::free_index();
            BaseType::set_free_index(index);
            BaseType::decr_used();
            return index;
        }
    }

    return 0;
}

template <typename POLICY>
const typename MemHashTable<POLICY>::ValueType& MemHashTable<POLICY>::deref(IntType index_) const
{
    return BaseType::value(index_ - 1);
}

template <typename POLICY>
typename MemHashTable<POLICY>::ValueType& MemHashTable<POLICY>::deref(IntType index_)
{
    return BaseType::value(index_ - 1);
}

template <typename POLICY>
const typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::begin() const
{
    return Iterator(this, find_first_used_bucket());
}

template <typename POLICY>
typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::begin()
{
    return Iterator(this, find_first_used_bucket());
}

template <typename POLICY>
typename MemHashTable<POLICY>::IntType MemHashTable<POLICY>::find_first_used_bucket() const
{
    IntType max_bucket_num = BaseType::buckets_num();
    for (IntType i = 0; i < max_bucket_num; ++i)
    {
        if (BaseType::buckets(i) != 0)
            return BaseType::buckets(i);
    }
    return 0;
}

template <typename POLICY>
const typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::end() const
{
    return Iterator(this, 0);
}

template <typename POLICY>
typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::end()
{
    return Iterator(this, 0);
}

template <typename POLICY>
const typename MemHashTable<POLICY>::ValueType& MemHashTable<POLICY>::Iterator::operator*() const
{
    return m_table->value(m_index - 1);
}

template <typename POLICY>
typename MemHashTable<POLICY>::ValueType& MemHashTable<POLICY>::Iterator::operator*()
{
    return const_cast<MemHashTable*>(m_table)->value(m_index - 1);
}

template <typename POLICY>
const typename MemHashTable<POLICY>::ValueType* MemHashTable<POLICY>::Iterator::operator->() const
{
    return &(operator*());
}

template <typename POLICY>
typename MemHashTable<POLICY>::ValueType* MemHashTable<POLICY>::Iterator::operator->()
{
    return &(operator*());
}

template <typename POLICY>
bool MemHashTable<POLICY>::Iterator::operator==(const Iterator& right_) const
{
    return (m_table == right_.m_table) && (m_index == right_.m_index);
}

template <typename POLICY>
bool MemHashTable<POLICY>::Iterator::operator!=(const Iterator& right_) const
{
    return (m_table != right_.m_table) || (m_index != right_.m_index);
}

template <typename POLICY>
typename MemHashTable<POLICY>::Iterator& MemHashTable<POLICY>::Iterator::operator++()
{
    IntType next_index = m_table->next(m_index - 1);
    if (next_index == 0)
    {
        // 该链上的最后一个节点，找下一个hash
        IntType next_bucket = m_table->get_bucket_index(m_table->key_of_value(m_table->value(m_index - 1))) + 1;
        IntType max_bucket_num = m_table->buckets_num();
        while (next_bucket < max_bucket_num)
        {
            if (m_table->buckets(next_bucket) != 0)
            {
                next_index = m_table->buckets(next_bucket);
                break;
            }
            ++next_bucket;
        }
    }

    m_index = next_index;
    return (*this);
}

template <typename POLICY>
typename MemHashTable<POLICY>::Iterator MemHashTable<POLICY>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

}  // namespace inner
}  // namespace pepper

#endif
