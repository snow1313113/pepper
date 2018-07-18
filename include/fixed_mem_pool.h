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
#include "../utils/traits_utils.h"
#include "base_struct.h"

namespace Pepper
{

template<typename T>
class FixedMemPool
{
private:
    struct Block
    {
        /// block 标志，0表示没有被使用
        size_t flag;
        /// 空闲时作为下一个节点的偏移量 这里不和obj组成一个union是为了考虑 obj非pod结构的情况
        Link<size_t> list;
        /// 非空闲是存储的节点数据
        T obj;
    };

    struct MemHeader
    {
        /// 共享内存版本号，当结构发现变化时可以做兼容处理，当前没什么用
        size_t version;
        /// 总内存大小
        size_t mem_size;
        /// 每个内存块大小
        size_t block_size;
        /// 节点T类型的大小
        size_t t_size;
        /// 已经使用了的原始内存大小（包括空闲链中的）
        size_t raw_used_offset;
        /// 已经被申请使用的内存大小
        size_t used_size;
        /// 当前使用中的内存块链表头节点，这个节点预留用来做标签，不使用的
        size_t used_list;
        /// 回收可再用的空闲内存块链表头
        size_t reclaim_list;
        /// 魔数
        size_t magic_num;
    };

public:
    class Iterator
    {
        size_t m_pos;
        const FixedMemPool * m_pool;
        Iterator(const FixedMemPool * pool_, size_t pos_):m_pos(pos_), m_pool(pool_)
        {
        }

    public:
        friend class FixedMemPool;
        Iterator() = default;

        const T & operator*() const
        {
            return m_pool->ref_2_ptr(m_pos)->obj;
        }
        T & operator*()
        {
            return m_pool->ref_2_ptr(m_pos)->obj;
        }

        const T * operator->() const
        {
            return &(operator*());
        }

        T * operator->()
        {
            return &(operator*());
        }

        bool operator==(const Iterator & right_) const
        {
            return (m_pool == right_.m_pool) && (m_pos == right_.m_pos);
        }
        bool operator!=(const Iterator & right_) const
        {
            return (m_pool != right_.m_pool) || (m_pos != right_.m_pos);
        }

        Iterator & operator++()
        {
            Block * p = m_pool->ref_2_ptr(m_pos);
            assert(p->flag == BLOCK_MAGIC_NUM);
            m_pos = p->list.prev;
            return (*this);
        }

        Iterator operator++(int)
        {
            Iterator temp = (*this);
            ++(*this);
            return temp;
        }

        Iterator & operator--()
        {
            Block * p = m_pool->ref_2_ptr(m_pos);
            assert(p->flag == BLOCK_MAGIC_NUM);
            m_pos = p->list.next;
            return (*this);
        }

        Iterator operator--(int)
        {
            Iterator temp = (*this);
            --(*this);
            return temp;
        }
    };

public:
    static size_t calc_need_size(size_t max_node_num_, size_t node_size_)
    {
        // todo 需要额外申请多一个节点，其实并不需要一整个节点，只要一个链表头部即可
        return (max_node_num_ + 1) * (sizeof(Block) + node_size_ - sizeof(T)) + sizeof(MemHeader);
    }

    static size_t calc_need_size(size_t max_node_num_)
    {
        return calc_need_size(max_node_num_, sizeof(T));
    }

    FixedMemPool():m_header(NULL){}
    ~FixedMemPool()
    {
        static_assert(std::is_trivial<T>::value, "type T must be trivial");
    }
    /// 初始化内存池，内存由调用者提供，is_raw_指明mem_指向的内存是否已经初始化过的
    bool init(void * mem_, size_t size_, bool is_raw_ = true);
    /// 初始化内存池，内存由调用者提供，指出节点大小（有可能大于或等于sizeof(T)），is_raw_指明mem_指向的内存是否已经初始化过的
    bool init(void * mem_, size_t size_, size_t node_size_, bool is_raw_ = true);
    /// 申请一个节点
    T * alloc(bool zero = true);
    /// 回收一个节点
    bool free(T * p_);
    /// 清空内存池
    void clear();
    /// 获得内存池中的第一个已经使用的元素的迭代器
    Iterator begin() const;
    /// 获取内存池的结尾迭代器
    Iterator end() const;
    /// 计算内存的使用率,百分比
    size_t get_mem_utilization() const;
    /// 获取最大节点个数
    size_t get_max_node_count() const;
    /// 获取已经分配的节点个数
    size_t get_alloc_node_count() const;

    inline size_t ptr_2_int(const T * p_) const
    {
        const Block * p = contaner_of(p_, &Block::obj);
        return ptr_2_ref(p);
    }

    inline T * int_2_ptr(size_t pos_) const
    {
        return &(ref_2_ptr(pos_)->obj);
    }

private:
    void init_header(size_t size_, size_t node_size_)
    {
        assert(m_header != NULL);
        m_header->version = VERSION;
        m_header->mem_size = size_;
        m_header->block_size = sizeof(Block) + node_size_ - sizeof(T);
        m_header->t_size = node_size_;
        m_header->raw_used_offset = sizeof(MemHeader);
        m_header->reclaim_list = 0;
        m_header->magic_num = HEADER_MAGIC_NUM;

        assert(m_header->raw_used_offset + m_header->block_size <= m_header->mem_size);
        Block * flag_node = reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(m_header) + m_header->raw_used_offset);
        assert(flag_node != NULL);

        m_header->raw_used_offset += m_header->block_size;

        flag_node->flag = BLOCK_MAGIC_NUM;
        size_t flag_ref = ptr_2_ref(flag_node);
        flag_node->list.next = flag_ref;
        flag_node->list.prev = flag_ref;
        m_header->used_list = flag_ref;
    }

    inline size_t ptr_2_ref(const void * p) const
    {
        return reinterpret_cast<const uint8_t *>(p) - reinterpret_cast<uint8_t *>(m_header);
    }

    inline Block * ref_2_ptr(size_t pos) const
    {
        return reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(m_header) + pos);
    }

private:
    // no used
    FixedMemPool(const FixedMemPool &);
    FixedMemPool & operator=(const FixedMemPool &);

private:
    static const size_t HEADER_MAGIC_NUM = 0x9E370001;
    static const size_t BLOCK_MAGIC_NUM = 0xAAAAAAAA;
    static const size_t VERSION = 1;

private:
    MemHeader * m_header;
};

template<typename T>
bool FixedMemPool<T>::init(void * mem_, size_t size_, bool is_raw_)
{
    return init(mem_, size_, sizeof(T), is_raw_);
}

template<typename T>
bool FixedMemPool<T>::init(void * mem_, size_t size_, size_t node_size_, bool is_raw_)
{
    if(NULL == mem_)
        return false;

    m_header = reinterpret_cast<MemHeader*>(mem_);
    if(is_raw_)
    {
        init_header(size_, node_size_);
    }

    if(m_header->magic_num != HEADER_MAGIC_NUM ||
            m_header->mem_size != size_ ||
            m_header->t_size != node_size_ ||
            m_header->block_size != sizeof(Block) + node_size_ - sizeof(T))
        return false;

    return true;
}

template<typename T>
T * FixedMemPool<T>::alloc(bool zero)
{
    Block * p = NULL;
    // 先从回收队列里面找，没有再去找一个新鲜的
    if(m_header->reclaim_list != 0)
    {
        p = ref_2_ptr(m_header->reclaim_list);
        m_header->reclaim_list = p->list.next;
    }
    else
    {
        if(m_header->raw_used_offset + m_header->block_size <= m_header->mem_size)
        {
            p = reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(m_header) + m_header->raw_used_offset);
            m_header->raw_used_offset += m_header->block_size;
        }
    }

    if(p == NULL)
        return NULL;

    if (zero)
        memset(p, 0, m_header->block_size);
    p->flag = BLOCK_MAGIC_NUM;

    // 插入到used链中
    assert(m_header->used_list != 0);

    Block * flag_node = ref_2_ptr(m_header->used_list);
    Block * next_node = ref_2_ptr(flag_node->list.next);
    p->list.next = flag_node->list.next;
    p->list.prev = next_node->list.prev;
    flag_node->list.next = ptr_2_ref(p);
    next_node->list.prev = ptr_2_ref(p);

    m_header->used_size += m_header->block_size;

    return &(p->obj);
}

template<typename T>
bool FixedMemPool<T>::free(T * p_)
{
    Block * p = contaner_of(p_, &Block::obj);
    if(p->flag != BLOCK_MAGIC_NUM)
        return false;

    // 从used链表上摘掉该节点
    Block * prev_node = ref_2_ptr(p->list.prev);
    Block * next_node = ref_2_ptr(p->list.next);
    prev_node->list.next = p->list.next;
    next_node->list.prev = p->list.prev;

    // 放入reclaim链中
    p->flag = 0;
    p->list.prev = 0;
    p->list.next = m_header->reclaim_list;
    m_header->reclaim_list = ptr_2_ref(p);
    m_header->used_size -= m_header->block_size;
    return true;
}

template<typename T>
void FixedMemPool<T>::clear()
{
    init(m_header, m_header->mem_size, m_header->t_size, true);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::begin() const
{
    Block * flag_node = ref_2_ptr(m_header->used_list);
    return Iterator(this, flag_node->list.prev);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::end() const
{
    return Iterator(this, m_header->used_list);
}

template<typename T>
size_t FixedMemPool<T>::get_mem_utilization() const
{
    return m_header->used_size * 100 / m_header->mem_size;
}

template<typename T>
size_t FixedMemPool<T>::get_max_node_count() const
{
    return (m_header->mem_size - sizeof(MemHeader)) / m_header->block_size - 1;
}

template<typename T>
size_t FixedMemPool<T>::get_alloc_node_count() const
{
    return m_header->used_size / m_header->block_size;
}

}

#endif
