/*
 * * file name: fix_mem_pool.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  2 07
 * */

#ifndef FIX_MEM_POOL_H
#define FIX_MEM_POOL_H

#include <string.h>
#include "../comm_macro.h"

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
        /// 空闲时作为下一个节点的偏移量 这里的next不和obj组成一个union是为了考虑 obj非pod结构的情况
        size_t next;
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
        size_t used_size;
        /// 当前使用中的内存块链表头
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
        const T & operator*() const
        {
            return m_pool->ref_2_ptr(m_pos)->obj;
        }
        T & operator*()
        {
            return m_pool->ref_2_ptr(m_pos)->obj;
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
            for(size_t i = m_pos + m_pool->m_header->block_size;
                    i < m_pool->m_header->used_size;
                    i += m_pool->m_header->block_size)
            {
                Block * p = m_pool->ref_2_ptr(i);
                if(p->flag == BLOCK_MAGIC_NUM)
                {
                    m_pos = i;
                    return (*this);
                }
            }

            m_pos = 0;
            return (*this);
        }

        Iterator operator++(int)
        {
            Iterator temp = (*this);
            for(size_t i = m_pos + m_pool->m_header->block_size;
                    i < m_pool->m_header->used_size;
                    i += m_pool->m_header->block_size)
            {
                Block * p = m_pool->ref_2_ptr(i);
                if(p->flag == BLOCK_MAGIC_NUM)
                {
                    m_pos = i;
                    return temp;
                }
            }

            m_pos = 0;
            return temp;
        }

        Iterator & operator--()
        {
            for(size_t i = m_pos;
                    i >= sizeof(MemHeader) + m_pool->m_header->block_size;)
            {
                i -= m_pool->m_header->block_size;
                Block * p = m_pool->ref_2_ptr(i);
                if(p->flag == BLOCK_MAGIC_NUM)
                {
                    m_pos = i;
                    return (*this);
                }
            }
            m_pos = 0;
            return (*this);
        }

        Iterator operator--(int)
        {
            Iterator temp = (*this);
            for(size_t i = m_pos;
                    i >= sizeof(MemHeader) + m_pool->m_header->block_size;)
            {
                i -= m_pool->m_header->block_size;
                Block * p = m_pool->ref_2_ptr(i);
                if(p->flag == BLOCK_MAGIC_NUM)
                {
                    m_pos = i;
                    return temp;
                }
            }

            m_pos = 0;
            return temp;
        }
    };

public:
    static size_t pool_header_size()
    {
        return sizeof(MemHeader);
    }

    static size_t block_header_size()
    {
        return sizeof(Block) - sizeof(T);
    }

    FixedMemPool():m_header(NULL){}
    ~FixedMemPool(){}
    /// 初始化内存池，is_raw_指明mem_指向的内存是否已经初始化过的
    bool init(void * mem_, size_t size_, bool is_raw_ = true);
    bool init(size_t size_);
    /// 申请一个节点
    T * alloc();
    /// 回收一个节点
    bool free(T * p_);
    /// 获得内存池中的第一个已经使用的元素的迭代器
    Iterator begin() const;
    /// 获取内存池的结尾迭代器
    Iterator end() const;

    inline size_t ptr_2_int(const T * p_) const
    {
        const Block * p = CONTAINER_OF(p_, Block, obj);
        return ptr_2_ref(p);
    }

    inline T * int_2_ptr(size_t pos_) const
    {
        return &(ref_2_ptr(pos_)->obj);
    }
private:
    void init_header(size_t size_)
    {
        assert(m_header != NULL);
        m_header->version = VERSION;
        m_header->mem_size = size_;
        m_header->block_size = sizeof(Block);
        m_header->t_size = sizeof(T);
        m_header->used_size = sizeof(MemHeader);
        m_header->reclaim_list = 0;
        m_header->magic_num = HEADER_MAGIC_NUM;
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
    if(NULL == mem_)
        return false;

    m_header = reinterpret_cast<MemHeader*>(mem_);
    if(is_raw_)
    {
        init_header(size_);
    }

    if(m_header->magic_num != HEADER_MAGIC_NUM ||
            m_header->mem_size != size_ ||
            m_header->t_size != sizeof(T) ||
            m_header->block_size != sizeof(Block))
        return false;

    return true;
}

template<typename T>
bool FixedMemPool<T>::init(size_t size_)
{
    m_header = new uint8_t[size_];
    init_header(size_);
}

template<typename T>
T * FixedMemPool<T>::alloc()
{
    Block * p = NULL;
    // 先从回收队列里面找，没有再去找一个新鲜的
    if(m_header->reclaim_list != 0)
    {
        p = ref_2_ptr(m_header->reclaim_list);
        m_header->reclaim_list = p->next;
    }
    else
    {
        if(m_header->used_size + m_header->block_size <= m_header->mem_size)
        {
            p = reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(m_header) + m_header->used_size);
            m_header->used_size += m_header->block_size;
        }
    }

    if(p == NULL)
        return NULL;

    memset(p, 0, m_header->block_size);
    p->flag = BLOCK_MAGIC_NUM;
    return &(p->obj);
}

template<typename T>
bool FixedMemPool<T>::free(T * p_)
{
    Block * p = CONTAINER_OF(p_, Block, obj);
    if(p->flag != BLOCK_MAGIC_NUM)
        return false;

    p->flag = 0;
    p->next = m_header->reclaim_list;
    m_header->reclaim_list = ptr_2_ref(p);
    return true;
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::begin() const
{
    for(size_t i = sizeof(MemHeader); i < m_header->used_size; i += m_header->block_size)
    {
        Block * p = reinterpret_cast<Block *>(reinterpret_cast<uint8_t *>(m_header) + i);
        if(p->flag == BLOCK_MAGIC_NUM)
            return Iterator(this, i);
    }
    return Iterator(this, 0);
}

template<typename T>
typename FixedMemPool<T>::Iterator FixedMemPool<T>::end() const
{
    return Iterator(this, 0);
}


}

#endif
