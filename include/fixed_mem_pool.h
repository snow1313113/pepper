/*
 * * file name: fixed_mem_pool.h
 * * description: 定长内存池
 * * author: lemonxu
 * * create time:2017  4 07
 * */

#ifndef _FIXED_MEM_POOL_H_
#define _FIXED_MEM_POOL_H_

#include <cstring>
#include <type_traits>
#include "utils/traits_utils.h"
#include "base_struct.h"

namespace Pepper
{

template<typename T>
class FixedMemPool
{
public:
    struct Iterator
    {
        // 按stl的做法，还是提供一个默认的构造函数，但是如果对这个迭代器做什么操作，结果未定义
        Iterator() = default;
        const T & operator*() const;
        T & operator*();
        const T * operator->() const;
        T * operator->();
        bool operator==(const Iterator & right_) const;
        bool operator!=(const Iterator & right_) const;
        Iterator & operator++();
        Iterator operator++(int);
        Iterator & operator--();
        Iterator operator--(int);
    private:
        friend class FixedMemPool;
        size_t m_index = 0;
        const FixedMemPool * m_pool = nullptr;
        Iterator(const FixedMemPool * pool_, size_t index_):m_index(index_),m_pool(pool_){}
    };

public:
    static size_t calc_need_size(size_t max_node_num_, size_t node_size_)
    {
        // LinkNode需要额外申请多一个节点作为头节点
        return sizeof(MemHeader) + max_node_num_ * node_size_ + (max_node_num_ + 1) * sizeof(LinkNode);
    }

    static size_t calc_need_size(size_t max_node_num_)
    {
        return calc_need_size(max_node_num_, sizeof(T));
    }

    FixedMemPool() = default;
    /// 初始化内存池，内存由调用者提供，is_raw_指明mem_指向的内存是否已经初始化过的
    bool init(void * mem_, size_t size_, bool is_raw_ = true);
    /// 初始化内存池，内存由调用者提供，指出节点大小（有可能大于或等于sizeof(T)），is_raw_指明mem_指向的内存是否已经初始化过的
    bool init(void * mem_, size_t size_, size_t node_size_, bool is_raw_ = true);
    /// 申请一个节点
    T * alloc(bool zero = true);
    /// 回收一个节点
    bool free(const T * p_);
    /// 清空内存池
    void clear();
    /// 是否满了
    bool full() const;
    /// 是否空了
    bool empty() const;
    /// 获得内存池中的第一个已经使用的元素的迭代器
    const Iterator begin() const;
    Iterator begin();
    /// 获取内存池的结尾迭代器
    const Iterator end() const;
    Iterator end();
    /// 计算内存的使用率,百分比
    size_t mem_utilization() const;
    /// 获取最大节点个数
    size_t capacity() const;
    /// 获取已经分配的节点个数
    size_t size() const;
    /// 节点大小
    size_t node_size() const;
    /// 根据value指针计算出是第几个节点，返回值[1, max_num]，0 则表示失败
    size_t ptr_2_int(const T * p_) const;
    const T * int_2_ptr(size_t index_) const;
    T * int_2_ptr(size_t index_);
    const Iterator int_2_iter(size_t index_) const;
    Iterator int_2_iter(size_t index_);

private:
    typedef Link<size_t> LinkNode;

    struct MemHeader
    {
        /// 共享内存版本号，当结构发现变化时可以做兼容处理，当前没什么用
        size_t version;
        /// 总内存大小
        size_t mem_size;
        /// 节点T类型的大小
        size_t t_size;
        /// 最大节点数
        size_t max_num;
        /// 已经被申请使用的节点数
        size_t used_num;
        /// 已经使用了的原始节点数
        size_t raw_used_num;
        /// 双向链表头节点位置
        size_t link_head_offset;
        /// 真正数据的开始位置
        size_t value_offset;
        /// 回收可再用的空闲内存块链表头
        size_t reclaim_list;
        /// 魔数
        size_t magic_num;
    };

private:
    // 初始化头部
    void init_header(size_t size_, size_t node_size_);
    // 根据一个下标值获取LinkNode，下标[0, max_num]
    const LinkNode * get_link(size_t index_) const;
    LinkNode * get_link(size_t index_);
    // 根据一个下标值获取T，下标[1, max_num]
    const T * get_value(size_t index_) const;
    T * get_value(size_t index_);
    // 根据内存大小计算能存的最大节点数
    static size_t calc_max_num(size_t size_, size_t node_size_)
    {
        if (size_ < sizeof(MemHeader) + sizeof(LinkNode))
            return 0;
        return (size_ - sizeof(MemHeader) - sizeof(LinkNode)) / (sizeof(LinkNode) + node_size_);
    }

private:
    FixedMemPool(const FixedMemPool &) = delete;
    FixedMemPool & operator=(const FixedMemPool &) = delete;
    static const size_t HEADER_MAGIC_NUM = 0x9E370001;
    static const size_t VERSION = 1;
private:
    MemHeader * m_header = nullptr;
};

template<typename T>
bool FixedMemPool<T>::init(void * mem_, size_t size_, bool is_raw_)
{
    return init(mem_, size_, sizeof(T), is_raw_);
}

template<typename T>
bool FixedMemPool<T>::init(void * mem_, size_t size_, size_t node_size_, bool is_raw_)
{
    if (NULL == mem_)
        return false;

    m_header = reinterpret_cast<MemHeader*>(mem_);
    if (is_raw_)
        init_header(size_, node_size_);

    if (m_header->magic_num != HEADER_MAGIC_NUM ||
            m_header->version != VERSION ||
            m_header->mem_size != size_ ||
            m_header->t_size != node_size_)
        return false;

    return true;
}

template<typename T>
T * FixedMemPool<T>::alloc(bool zero)
{
    if (full())
        return NULL;

    size_t index = 0;
    LinkNode * empty_node = NULL;
    // 先从回收队列里面找，没有再去找一个新鲜的
    if (m_header->reclaim_list != 0)
    {
        index = m_header->reclaim_list;
        empty_node = get_link(index);
        m_header->reclaim_list = empty_node->next;
    }
    else
    {
        assert(m_header->raw_used_num < m_header->max_num);
        empty_node = get_link(m_header->raw_used_num + 1);
        index = m_header->raw_used_num + 1;
        ++(m_header->raw_used_num);
    }

    assert(index > 0);
    assert(empty_node);

    // 插入到used链中
    LinkNode * head_node = get_link(0);
    LinkNode * next_node = get_link(head_node->next);
    empty_node->next = head_node->next;
    empty_node->prev = 0;
    next_node->prev = index;
    head_node->next = index;

    ++(m_header->used_num);

    T * p = get_value(index);
    // 这里可以不用memset，这样就可以在这里放入C++对象了
    if (zero)
        memset(p, 0, m_header->t_size);
    return p;
}

template<typename T>
bool FixedMemPool<T>::free(const T * p_)
{
    if (empty())
        return false;

    size_t index = ptr_2_int(p_);
    if (index == 0 || index > m_header->max_num)
        return false;

    // 进一步检查节点是否在使用，要考虑重复free的场景
    if (index > m_header->raw_used_num)
        return false;
    // 可以判断prev是否是大于max_num
    LinkNode * del_node = get_link(index);
    if (del_node->prev > m_header->max_num)
        return false;

    // 从used链表上摘掉该节点
    LinkNode * prev_node = get_link(del_node->prev);
    LinkNode * next_node = get_link(del_node->next);
    prev_node->next = del_node->next;
    next_node->prev = del_node->prev;

    // 放入reclaim链中，这个利用prev放入一个大于max_num的数，作为已经被回收的标记
    del_node->prev = m_header->max_num + 1;
    del_node->next = m_header->reclaim_list;
    m_header->reclaim_list = index;
    --(m_header->used_num);
    return true;
}

template<typename T>
void FixedMemPool<T>::clear()
{
    init(m_header, m_header->mem_size, m_header->t_size, true);
}

template<typename T>
bool FixedMemPool<T>::full() const
{
    return m_header->used_num >= m_header->max_num;
}

template<typename T>
bool FixedMemPool<T>::empty() const
{
    return m_header->used_num == 0;
}

template<typename T>
const typename FixedMemPool<T>::Iterator FixedMemPool<T>::begin() const
{
    const LinkNode * head_node = get_link(0);
    return Iterator(this, head_node->next);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::begin()
{
    const LinkNode * head_node = get_link(0);
    return Iterator(this, head_node->next);
}

template<typename T>
const typename FixedMemPool<T>::Iterator FixedMemPool<T>::end() const
{
    return Iterator(this, 0);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::end()
{
    return Iterator(this, 0);
}

template<typename T>
size_t FixedMemPool<T>::mem_utilization() const
{
    return m_header->used_num * (m_header->t_size + sizeof(LinkNode)) * 100 / m_header->mem_size;
}

template<typename T>
size_t FixedMemPool<T>::capacity() const
{
    return m_header->max_num;
}

template<typename T>
size_t FixedMemPool<T>::size() const
{
    return m_header->used_num;
}

template<typename T>
size_t FixedMemPool<T>::node_size() const
{
    return m_header->t_size;
}

template<typename T>
size_t FixedMemPool<T>::ptr_2_int(const T * p_) const
{
    const uint8_t * start_mem = reinterpret_cast<const uint8_t *>(m_header);
    if (reinterpret_cast<const uint8_t * >(p_) < start_mem + m_header->value_offset)
        return 0;
    size_t offset = reinterpret_cast<const uint8_t *>(p_) - start_mem - m_header->value_offset;
    if (offset % m_header->t_size != 0)
        return 0;
    return (offset / m_header->t_size) + 1;
}

template<typename T>
const T * FixedMemPool<T>::int_2_ptr(size_t index_) const
{
    return get_value(index_);
}

template<typename T>
T * FixedMemPool<T>::int_2_ptr(size_t index_)
{
    return get_value(index_);
}

template<typename T>
const typename FixedMemPool<T>::Iterator FixedMemPool<T>::int_2_iter(size_t index_) const
{
    return Iterator(this, index_);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::int_2_iter(size_t index_)
{
    return Iterator(this, index_);
}

template<typename T>
void FixedMemPool<T>::init_header(size_t size_, size_t node_size_)
{
    assert(m_header != NULL);
    m_header->version = VERSION;
    m_header->mem_size = size_;
    m_header->t_size = node_size_;
    m_header->max_num = FixedMemPool::calc_max_num(size_, node_size_);
    m_header->used_num = 0;
    m_header->raw_used_num = 0;
    m_header->link_head_offset = sizeof(MemHeader);
    m_header->value_offset = sizeof(MemHeader) + (m_header->max_num + 1) * sizeof(LinkNode);
    m_header->reclaim_list = 0;
    m_header->magic_num = HEADER_MAGIC_NUM;
    LinkNode * head_node = get_link(0);
    head_node->prev = 0;
    head_node->next = 0;
}

template<typename T>
const typename FixedMemPool<T>::LinkNode * FixedMemPool<T>::get_link(size_t index_) const
{
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->link_head_offset + index_ * sizeof(LinkNode);
    return reinterpret_cast<const LinkNode *>(reinterpret_cast<const uint8_t *>(m_header) + offset);
}

template<typename T>
typename FixedMemPool<T>::LinkNode * FixedMemPool<T>::get_link(size_t index_)
{
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->link_head_offset + index_ * sizeof(LinkNode);
    return reinterpret_cast<LinkNode *>(reinterpret_cast<uint8_t *>(m_header) + offset);
}

template<typename T>
const T * FixedMemPool<T>::get_value(size_t index_) const
{
    assert(index_ > 0);
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->value_offset + (index_ - 1) * m_header->t_size;
    return reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(m_header) + offset);
}

template<typename T>
T * FixedMemPool<T>::get_value(size_t index_)
{
    assert(index_ > 0);
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->value_offset + (index_ - 1) * m_header->t_size;
    return reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(m_header) + offset);
}

template<typename T>
const T & FixedMemPool<T>::Iterator::operator*() const
{
    return *(operator->());
}

template<typename T>
T & FixedMemPool<T>::Iterator::operator*()
{
    return *(operator->());
}

template<typename T>
const T * FixedMemPool<T>::Iterator::operator->() const
{
    return m_pool->get_value(m_index);
}

template<typename T>
T * FixedMemPool<T>::Iterator::operator->()
{
    return const_cast<FixedMemPool*>(m_pool)->get_value(m_index);
}

template<typename T>
bool FixedMemPool<T>::Iterator::operator==(const Iterator & right_) const
{
    return (m_pool == right_.m_pool) && (m_index == right_.m_index);
}

template<typename T>
bool FixedMemPool<T>::Iterator::operator!=(const Iterator & right_) const
{
    return (m_pool != right_.m_pool) || (m_index != right_.m_index);
}

template<typename T>
typename FixedMemPool<T>::Iterator & FixedMemPool<T>::Iterator::operator++()
{
    const LinkNode * node = m_pool->get_link(m_index);
    assert(node->prev <= m_pool->m_header->max_num);
    m_index = node->next;
    return (*this);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::Iterator::operator++(int)
{
    Iterator temp = (*this);
    ++(*this);
    return temp;
}

template<typename T>
typename FixedMemPool<T>::Iterator & FixedMemPool<T>::Iterator::operator--()
{
    const LinkNode * node = m_pool->get_link(m_index);
    assert(node->prev <= m_pool->m_header->max_num);
    m_index = node->prev;
    return (*this);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::Iterator::operator--(int)
{
    Iterator temp = (*this);
    --(*this);
    return temp;
}

}

#endif
