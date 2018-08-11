/*
 * * file name: fixed_mem_pool.h
 * * description: 定长内存池
 * * author: lemonxu
 * * create time:2017  4 07
 * */

#ifndef FIXED_MEM_POOL_H
#define FIXED_MEM_POOL_H

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
        size_t m_index;
        const FixedMemPool * m_pool;
        Iterator(const FixedMemPool * pool_, size_t index_):m_index(index_),m_pool(pool_){}
    };

public:
    static size_t CalcNeedSize(size_t max_node_num_, size_t node_size_)
    {
        // LinkNode需要额外申请多一个节点作为头节点
        return sizeof(MemHeader) + max_node_num_ * node_size_ + (max_node_num_ + 1) * sizeof(LinkNode);
    }

    static size_t CalcNeedSize(size_t max_node_num_)
    {
        return CalcNeedSize(max_node_num_, sizeof(T));
    }

    FixedMemPool():m_header(NULL){}
    /// 初始化内存池，内存由调用者提供，is_raw_指明mem_指向的内存是否已经初始化过的
    bool Init(void * mem_, size_t size_, bool is_raw_ = true);
    /// 初始化内存池，内存由调用者提供，指出节点大小（有可能大于或等于sizeof(T)），is_raw_指明mem_指向的内存是否已经初始化过的
    bool Init(void * mem_, size_t size_, size_t node_size_, bool is_raw_ = true);
    /// 申请一个节点
    T * Alloc(bool zero = true);
    /// 回收一个节点
    bool Free(const T * p_);
    /// 清空内存池
    void Clear();
    /// 是否满了
    bool IsFull() const;
    /// 是否空了
    bool IsEmpty() const;
    /// 获得内存池中的第一个已经使用的元素的迭代器
    const Iterator Begin() const;
    Iterator Begin();
    /// 获取内存池的结尾迭代器
    const Iterator End() const;
    Iterator End();
    /// 计算内存的使用率,百分比
    size_t GetMemUtilization() const;
    /// 获取最大节点个数
    size_t Capacity() const;
    /// 获取已经分配的节点个数
    size_t Size() const;

    // 返回整数，为什么要做这个，是为了兼容KR以前版本的那个MemPool的坑
    // 等哪天把那个坑改了，就可以把这几个接口删了
    // 实际中，在共享内存应用中，迭代器在进程重启后会无效，而整数不会，
    // 只不过整数不能做其它操作而已
    size_t Ptr2Int(const T * p_) const;
    const T * Int2Ptr(size_t index_) const;
    T * Int2Ptr(size_t index_);

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
    void InitHeader(size_t size_, size_t node_size_);
    // 根据一个下标值获取LinkNode，下标[0, max_num]
    const LinkNode * GetLink(size_t index_) const;
    LinkNode * GetLink(size_t index_);
    // 根据一个下标值获取T，下标[1, max_num]
    const T * GetValue(size_t index_) const;
    T * GetValue(size_t index_);
    // 根据value指针计算出是第几个节点，返回值[1, max_num]，0 则表示失败
    size_t Ptr2Index(const T * p_) const;
    // 根据内存大小计算能存的最大节点数
    static size_t CalcMaxNum(size_t size_, size_t node_size_)
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
    MemHeader * m_header;
};

template<typename T>
bool FixedMemPool<T>::Init(void * mem_, size_t size_, bool is_raw_)
{
    return Init(mem_, size_, sizeof(T), is_raw_);
}

template<typename T>
bool FixedMemPool<T>::Init(void * mem_, size_t size_, size_t node_size_, bool is_raw_)
{
    if (NULL == mem_)
        return false;

    m_header = reinterpret_cast<MemHeader*>(mem_);
    if (is_raw_)
        InitHeader(size_, node_size_);

    if (m_header->magic_num != HEADER_MAGIC_NUM ||
            m_header->version != VERSION ||
            m_header->mem_size != size_ ||
            m_header->t_size != node_size_)
        return false;

    return true;
}

template<typename T>
T * FixedMemPool<T>::Alloc(bool zero)
{
    if (IsFull())
        return NULL;

    size_t index = 0;
    LinkNode * empty_node = NULL;
    // 先从回收队列里面找，没有再去找一个新鲜的
    if (m_header->reclaim_list != 0)
    {
        index = m_header->reclaim_list;
        empty_node = GetLink(index);
        m_header->reclaim_list = empty_node->next;
    }
    else
    {
        assert(m_header->raw_used_num < m_header->max_num);
        empty_node = GetLink(m_header->raw_used_num + 1);
        index = m_header->raw_used_num + 1;
        ++(m_header->raw_used_num);
    }

    assert(index > 0);
    assert(empty_node);

    // 插入到used链中
    LinkNode * head_node = GetLink(0);
    LinkNode * next_node = GetLink(head_node->next);
    empty_node->next = head_node->next;
    empty_node->prev = 0;
    next_node->prev = index;
    head_node->next = index;

    ++(m_header->used_num);

    T * p = GetValue(index);
    // 这里可以不用memset，这样就可以在这里放入C++对象了
    if (zero)
        memset(p, 0, m_header->t_size);
    return p;
}

template<typename T>
bool FixedMemPool<T>::Free(const T * p_)
{
    if (IsEmpty())
        return false;

    size_t index = Ptr2Index(p_);
    if (index == 0 || index > m_header->max_num)
        return false;

    // 进一步检查节点是否在使用，要考虑重复free的场景
    if (index > m_header->raw_used_num)
        return false;
    // 可以判断prev是否是大于max_num
    LinkNode * del_node = GetLink(index);
    if (del_node->prev > m_header->max_num)
        return false;

    // 从used链表上摘掉该节点
    LinkNode * prev_node = GetLink(del_node->prev);
    LinkNode * next_node = GetLink(del_node->next);
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
void FixedMemPool<T>::Clear()
{
    Init(m_header, m_header->mem_size, m_header->t_size, true);
}

template<typename T>
bool FixedMemPool<T>::IsFull() const
{
    return m_header->used_num >= m_header->max_num;
}

template<typename T>
bool FixedMemPool<T>::IsEmpty() const
{
    return m_header->used_num == 0;
}

template<typename T>
const typename FixedMemPool<T>::Iterator FixedMemPool<T>::Begin() const
{
    const LinkNode * head_node = GetLink(0);
    return Iterator(this, head_node->next);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::Begin()
{
    const LinkNode * head_node = GetLink(0);
    return Iterator(this, head_node->next);
}

template<typename T>
const typename FixedMemPool<T>::Iterator FixedMemPool<T>::End() const
{
    return Iterator(this, 0);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::End()
{
    return Iterator(this, 0);
}

template<typename T>
size_t FixedMemPool<T>::GetMemUtilization() const
{
    return m_header->used_num * (m_header->t_size + sizeof(LinkNode)) * 100 / m_header->mem_size;
}

template<typename T>
size_t FixedMemPool<T>::Capacity() const
{
    return m_header->max_num;
}

template<typename T>
size_t FixedMemPool<T>::Size() const
{
    return m_header->used_num;
}

template<typename T>
inline size_t FixedMemPool<T>::Ptr2Int(const T * p_) const
{
    return Ptr2Index(p_);
}

template<typename T>
const T * FixedMemPool<T>::Int2Ptr(size_t index_) const
{
    return GetValue(index_);
}

template<typename T>
T * FixedMemPool<T>::Int2Ptr(size_t index_)
{
    return GetValue(index_);
}

template<typename T>
void FixedMemPool<T>::InitHeader(size_t size_, size_t node_size_)
{
    assert(m_header != NULL);
    m_header->version = VERSION;
    m_header->mem_size = size_;
    m_header->t_size = node_size_;
    m_header->max_num = FixedMemPool::CalcMaxNum(size_, node_size_);
    m_header->used_num = 0;
    m_header->raw_used_num = 0;
    m_header->link_head_offset = sizeof(MemHeader);
    m_header->value_offset = sizeof(MemHeader) + (m_header->max_num + 1) * sizeof(LinkNode);
    m_header->reclaim_list = 0;
    m_header->magic_num = HEADER_MAGIC_NUM;
    LinkNode * head_node = GetLink(0);
    head_node->prev = 0;
    head_node->next = 0;
}

template<typename T>
const typename FixedMemPool<T>::LinkNode * FixedMemPool<T>::GetLink(size_t index_) const
{
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->link_head_offset + index_ * sizeof(LinkNode);
    return reinterpret_cast<const LinkNode *>(reinterpret_cast<const uint8_t *>(m_header) + offset);
}

template<typename T>
typename FixedMemPool<T>::LinkNode * FixedMemPool<T>::GetLink(size_t index_)
{
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->link_head_offset + index_ * sizeof(LinkNode);
    return reinterpret_cast<LinkNode *>(reinterpret_cast<uint8_t *>(m_header) + offset);
}

template<typename T>
const T * FixedMemPool<T>::GetValue(size_t index_) const
{
    assert(index_ > 0);
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->value_offset + (index_ - 1) * m_header->t_size;
    return reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(m_header) + offset);
}

template<typename T>
T * FixedMemPool<T>::GetValue(size_t index_)
{
    assert(index_ > 0);
    assert(index_ <= m_header->max_num);
    size_t offset = m_header->value_offset + (index_ - 1) * m_header->t_size;
    return reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(m_header) + offset);
}

template<typename T>
size_t FixedMemPool<T>::Ptr2Index(const T * p_) const
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
    return m_pool->GetValue(m_index);
}

template<typename T>
T * FixedMemPool<T>::Iterator::operator->()
{
    return const_cast<FixedMemPool*>(m_pool)->GetValue(m_index);
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
    const LinkNode * node = m_pool->GetLink(m_index);
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
    const LinkNode * node = m_pool->GetLink(m_index);
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
