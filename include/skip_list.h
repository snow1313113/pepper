/*
 * * file name: mem_rank.h
 * * description: 用跳跃表加哈希表实现的一个实时排名模板类
 * * author: lemonxu
 * * create time: 2016-六月-13
 * */

#ifndef MEM_RANK_H
#define MEM_RANK_H

#include <vector>
#include "inner/head.h"
#include "utils/traits_utils.h"
using std::vector;

namespace Pepper
{

// 对于自定义复合类型T，需要特化两个类，ExtractKey和std::less
// 或者自己实现类似的类作为模板参数也可以
template<typename T,
    typename T_Key = ExtractKey<T>,
    typename T_Compare = std::less<T> >
class MemRank
{
private:
    typedef typename T_Key::KeyType KeyType;
    static const size_t SKIPTABLE_P = 10;

    struct MRNode
    {
        T info;
        /// 节点跳跃个数
        size_t span;
        /// 四个方向的链表指针
        size_t back;
        size_t forward;
        size_t a;        
        size_t up;
        size_t down;
        /// 哈希桶中的链表指针，空闲链表中时指向下一个空闲节点
        size_t next;
    };

    struct MRHeader
    {
        /// 总内存大小
        size_t mem_size;
        /// 每个内存块大小
        size_t block_size;
        /// 存储的节点T类型大小
        size_t t_size;
        /// 哈希桶头部的偏移位置
        size_t hash_head_ref;
        /// 哈希桶的个数
        size_t bucket_num;
        /// 跳跃表的起始位置
        size_t level_ref;
        /// 跳跃表的层数
        size_t level_num;
        /// 空闲内存链表的头节点位置
        size_t free_list;
        /// 空闲节点个数
        size_t free_num;
        /// 存储的T的个数，等同于最低层的节点个数
        size_t t_num;
        /// 真正节点数据开始的偏移
        size_t node_ref;
        /// 魔数
        size_t magic_num;
    };

    static const size_t MAGIC_NUM = 0x12345678;
    MRHeader * m_header;
    MRNode * m_hash;
    MRNode * m_level;
    MRNode * m_free;

public:
    class Iterator
    {
        size_t m_node_ref;
        const MemRank * m_rank;
        Iterator(const MemRank * rank_, size_t ref_):m_node_ref(ref_), m_rank(rank_)
        {
        }

    public:
        friend class MemRank;
        Iterator() = default;

        const T & operator*() const
        {
            return m_rank->ref_2_node(m_node_ref)->info;
        }
        bool operator==(const Iterator & right_) const
        {
            return (m_rank == right_.m_rank) && (m_node_ref == right_.m_node_ref);
        }
        bool operator!=(const Iterator & right_) const
        {
            return (m_rank != right_.m_rank) || (m_node_ref != right_.m_node_ref);
        }

        Iterator & operator++()
        {
            m_node_ref = m_rank->ref_2_node(m_node_ref)->back;
            return (*this);
        }

        Iterator operator++(int)
        {
            Iterator temp = (*this);
            m_node_ref = m_rank->ref_2_node(m_node_ref)->back;
            return temp;
        }

        Iterator & operator--()
        {
            m_node_ref = m_rank->ref_2_node(m_node_ref)->forward;
            return (*this);
        }

        Iterator operator--(int)
        {
            Iterator temp = (*this);
            m_node_ref = m_rank->ref_2_node(m_node_ref)->forward;
            return temp;
        }
    };

public:

    /// 初始化
    bool init(void * mem_,
            size_t size_,
            bool is_raw_ = true,
            size_t level_num_ = 10,
            size_t bucket_num_ = 1000003);
    /// 返回使用的空闲节点个数
    size_t get_free_node() const
    {
        return m_header->free_num;
    }
    /// 返回插入的元素个数
    size_t size() const
    {
        return m_header->t_num;
    }
    /// 更新一个节点，如果没有存在，则插入
    bool update_node(const T & info_);
    /// 插入一个节点，不判断是否存在
    bool insert_node(const T & info_);
    /// 删除一个节点
    bool delete_node(const KeyType & key_);
    bool delete_node(const T & info_);
    /// 获取对应节点的排名
    size_t get_rank(const T & info_) const;
    size_t get_rank(const KeyType & key_, T & info_) const;
    /// 获得排在最前面的N个节点，从高到低
    size_t get_top_n(size_t n_, vector<T> & vec_) const;
    /// 获取排在最后面的N个节点，从低到高
    size_t get_last_n(size_t n_, vector<T> & vec_) const;
    /// 获取对应节点的前面n个节点
    bool get_pre_n(const KeyType & key_, size_t n_, vector<T> & vec_) const;
    /// 获取对应节点的后面n个节点
    bool get_next_n(const KeyType & key_, size_t n_, vector<T> & vec_) const;

    /// 获取排行榜列表开始的迭代器
    Iterator begin() const;
    /// 获取排行榜列表结尾的迭代器
    Iterator end() const;

    /// 黑名单接口，晚点实现 todo
    bool add_black_list(const vector<T> & black_list_);
    bool delete_back_list(const vector<T> & black_list_);

private:
    inline size_t ptr_2_ref(void * ptr_) const
    {
        return reinterpret_cast<uint8_t *>(ptr_) - reinterpret_cast<uint8_t *>(m_header);
    }

    inline uint8_t * ref_2_ptr(size_t ref_) const
    {
        return reinterpret_cast<uint8_t *>(m_header) + ref_;
    }

    inline MRNode * ref_2_node(size_t ref_) const
    {
        return reinterpret_cast<MRNode *>(reinterpret_cast<uint8_t *>(m_header) + ref_);
    }

    inline size_t * ref_2_uint(size_t ref_) const
    {
        return reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(m_header) + ref_);
    }
    size_t align(size_t byte_size_) const
    {
        // todo
        return byte_size_;
    }

    inline size_t random_max_level() const
    {
        size_t level = 1;
        while ((random() & 0xFFFF) < (1.0 / SKIPTABLE_P * 0xFFFF))
            level += 1;
        return (level < m_header->level_num) ? level : m_header->level_num;
    }

    /// 查找对应节点
    MRNode * find(const KeyType & key_) const;
    bool delete_node(MRNode * node_);
    MRNode * alloc_node();
    void free_node(MRNode * node_);
};

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::init(void * mem_,
            size_t size_,
            bool is_raw_,
            size_t level_num_,
            size_t bucket_num_)
{
    if(NULL == mem_)
        return false;

    m_header = reinterpret_cast<MRHeader*>(mem_);
    if(is_raw_)
    {
        // 内存连头部信息都存不下
        if(size_ < (sizeof(MRHeader) + sizeof(size_t) * bucket_num_ + sizeof(MRNode) * level_num_))
            return false;

        m_header->mem_size = size_;
        m_header->block_size = align(sizeof(MRNode));
        m_header->t_size = sizeof(T);
        m_header->hash_head_ref = ptr_2_ref(reinterpret_cast<uint8_t*>(m_header) + sizeof(MRHeader));
        m_header->bucket_num = bucket_num_;

        m_header->level_ref = ptr_2_ref(reinterpret_cast<uint8_t*>(m_header) + sizeof(MRHeader) + bucket_num_ * sizeof(MRNode *));
        m_header->level_num = level_num_;
        for(size_t i = 0; i < level_num_; ++i)
        {
            size_t ref = m_header->level_ref + i * sizeof(MRNode);
            MRNode * node = ref_2_node(ref);
            node->span = 1;
            node->back = ref;
            node->forward = ref;
            if(i + 1 == level_num_)
                node->up = 0;
            else
                node->up = m_header->level_ref + (i + 1) * sizeof(MRNode);

            if(i == 0)
                node->down = 0;
            else
                node->down = m_header->level_ref + (i - 1) * sizeof(MRNode);
        }

        m_header->node_ref =  ptr_2_ref(reinterpret_cast<uint8_t*>(m_header) +
                sizeof(MRHeader) +
                bucket_num_ * sizeof(size_t) +
                level_num_ * sizeof(MRNode));

        // 还没初始化的内存，所有的节点都是空闲节点，要连起来
        m_header->free_list = 0;
        m_header->free_num = 0;
        uint8_t * node_ptr = ref_2_ptr(m_header->node_ref);
        size_t free_size = size_ - sizeof(MRHeader) - sizeof(size_t) * bucket_num_ - sizeof(MRNode) * level_num_;
        while(free_size >= m_header->block_size)
        {
            MRNode * p = reinterpret_cast<MRNode *>(node_ptr + m_header->free_num * m_header->block_size);
            p->next = m_header->free_list;
            m_header->free_list = ptr_2_ref(p);
            ++(m_header->free_num);
            free_size -= m_header->block_size;
        }

        m_header->t_num = 0;
        m_header->magic_num = MAGIC_NUM;
    }
    else
    {
        if(m_header->magic_num != MAGIC_NUM ||
                m_header->mem_size != size_ ||
                m_header->t_size != sizeof(T) ||
                m_header->block_size != align(sizeof(MRNode)))
            return false;
    }

    return true;
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::update_node(const T & info_)
{
    MRNode * p = find(T_Key()(info_));
    if(p != NULL)
        delete_node(p);
    return insert_node(info_);
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::insert_node(const T & info_)
{
    T_Compare compare;

    size_t level_count = m_header->level_num;

    // 向下查找过程中保存每层有可能被插入的位置
    vector<MRNode *> temp_nodes;
    temp_nodes.reserve(level_count);

    // 从最高层找起
    for(size_t level_ref = m_header->level_ref + (level_count - 1) * sizeof(MRNode);
            level_ref != 0;)
    {
        MRNode * level_node = ref_2_node(level_ref);
        MRNode * node = ref_2_node(level_node->back);
        while(node != level_node && compare(info_, node->info))
        {
            node = ref_2_node(node->back);
        }

        temp_nodes.push_back(node);
        --level_count;
        level_ref = (ref_2_node(node->forward))->down;
    }

    assert(level_count == 0);

    // 开始从最低层插入，保证底层优先插入成功
    size_t max_level = random_max_level();
    assert(temp_nodes.size() >= max_level);
    size_t span = 1;
    size_t level = 0;
    MRNode * new_node = NULL;
    for(MRNode * down_node = NULL; level < max_level; ++level)
    {
        new_node = alloc_node();
        // 就算获取新节点失败，也只是表示没有插入到高层
        if(new_node == NULL)
            return false;
        MRNode * node = temp_nodes[m_header->level_num - level - 1];
        MRNode * forward_node = ref_2_node(node->forward);

        new_node->info = info_;
        new_node->span = node->span + 1 - span;
        new_node->back = forward_node->back;
        new_node->forward = node->forward;
        node->span = span;
        node->forward = ptr_2_ref(new_node);
        forward_node->back = node->forward;

        if(down_node != NULL)
        {
            new_node->down = ptr_2_ref(down_node);
            down_node->up = ptr_2_ref(new_node);
        }
        down_node = new_node;

        // 计算上一层可能插入节点到其下一节点的span
        if(level + 1 < max_level)
        {
            MRNode * temp_node = new_node;
            span = 0;
            do
            {
                assert(temp_node->back != 0);
                temp_node = ref_2_node(temp_node->back);
                span += temp_node->span;
            }while(temp_node->up == 0);
        }
    }

    assert(new_node != NULL);
    assert(new_node->up == 0);

    // 剩余不需要插入节点的层调整span
    for(; level < m_header->level_num; ++level)
    {
        temp_nodes[m_header->level_num - level - 1]->span += 1;
    }

    // 插入hash表中
    const KeyType & key = T_Key()(info_);
    size_t * slot_ref = ref_2_uint(m_header->hash_head_ref + (key % m_header->bucket_num) * sizeof(size_t));
    new_node->next = *slot_ref;
    *slot_ref = ptr_2_ref(new_node);

    ++(m_header->t_num);

    return true;
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::delete_node(const KeyType & key_)
{
    MRNode * p = find(key_);
    if(p != NULL)
        return delete_node(p);
    return true;
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::delete_node(const T & info_)
{
    return delete_node(T_Key()(info_));
}

template<typename T, typename T_Key, typename T_Compare>
size_t MemRank<T, T_Key, T_Compare>::get_rank(const KeyType & key_, T & info_) const
{
    MRNode * p = find(key_);
    if(p == NULL)
        return 0;

    assert(p->up == 0);

    size_t total_span = p->span;
    size_t next_ref = p->forward;
    while(next_ref != 0)
    {
        MRNode * next_node = ref_2_node(next_ref);
        // key如果为0 表示用于标记的头节点，不用继续找了
        if(T_Key()(next_node->info) == 0)
            break;

        if(next_node->up != 0)
        {
            next_ref = next_node->up;
        }
        else
        {
            total_span += next_node->span;
            next_ref = next_node->forward;
        }
    }
    info_ = p->info;
    return total_span;
}

template<typename T, typename T_Key, typename T_Compare>
size_t MemRank<T, T_Key, T_Compare>::get_rank(const T & info_) const
{
    MRNode * p = find(T_Key()(info_));
    if(p == NULL)
        return 0;

    assert(p->up == 0);

    size_t total_span = p->span;
    size_t next_ref = p->forward;
    while(next_ref != 0)
    {
        MRNode * next_node = ref_2_node(next_ref);
        // key如果为0 表示用于标记的头节点，不用继续找了
        if(T_Key()(next_node->info) == 0)
            break;

        if(next_node->up != 0)
        {
            next_ref = next_node->up;
        }
        else
        {
            total_span += next_node->span;
            next_ref = next_node->forward;
        }
    }
    return total_span;
}

template<typename T, typename T_Key, typename T_Compare>
size_t MemRank<T, T_Key, T_Compare>::get_top_n(size_t n_, vector<T> & vec_) const
{
    size_t level_ref = m_header->level_ref;
    for(MRNode * next_node = ref_2_node(level_ref);
            n_ != 0 && next_node->back != level_ref;
            --n_)
    {
        next_node = ref_2_node(next_node->back);
        vec_.push_back(next_node->info);
    }
    return vec_.size();
}

template<typename T, typename T_Key, typename T_Compare>
size_t MemRank<T, T_Key, T_Compare>::get_last_n(size_t n_, vector<T> & vec_) const
{
    size_t level_ref = m_header->level_ref;
    for(MRNode * next_node = ref_2_node(level_ref);
            n_ != 0 && next_node->forward != level_ref;
            --n_)
    {
        next_node = ref_2_node(next_node->forward);
        vec_.push_back(next_node->info);
    }
    return vec_.size();
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::get_pre_n(const KeyType & key_, size_t n_, vector<T> & vec_) const
{
    MRNode * p = find(key_);
    if(p == NULL)
        return false;

    assert(p->up == 0);
    while(p->down != 0)
    {
        p = ref_2_node(p->down);
    }

    for(size_t level_ref = m_header->level_ref;
            n_ != 0 && p->forward != level_ref;
            --n_)
    {
        p = ref_2_node(p->forward);
        vec_.push_back(p->info);
    }

    return true;
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::get_next_n(const KeyType & key_, size_t n_, vector<T> & vec_) const
{
    MRNode * p = find(key_);
    if(p == NULL)
        return false;

    assert(p->up == 0);
    while(p->down != 0)
    {
        p = ref_2_node(p->down);
    }

    for(size_t level_ref = m_header->level_ref;
            n_ != 0 && p->back != level_ref;
            --n_)
    {
        p = ref_2_node(p->back);
        vec_.push_back(p->info);
    }

    return true;
}

template<typename T, typename T_Key, typename T_Compare>
typename MemRank<T, T_Key, T_Compare>::Iterator MemRank<T, T_Key, T_Compare>::begin() const
{
    size_t level_ref = m_header->level_ref;
    assert(level_ref != 0);
    return Iterator(this, ref_2_node(level_ref)->back);
}

template<typename T, typename T_Key, typename T_Compare>
typename MemRank<T, T_Key, T_Compare>::Iterator MemRank<T, T_Key, T_Compare>::end() const
{
    assert(m_header->level_ref != 0);
    return Iterator(this, m_header->level_ref);
}

template<typename T, typename T_Key, typename T_Compare>
typename MemRank<T, T_Key, T_Compare>::MRNode * MemRank<T, T_Key, T_Compare>::find(const KeyType & key_) const
{
    size_t slot_ref = *(ref_2_uint(m_header->hash_head_ref + (key_ % m_header->bucket_num) * sizeof(size_t)));
    while(slot_ref != 0)
    {
        MRNode * slot_header = ref_2_node(slot_ref);
        if(T_Key()(slot_header->info) == key_)
            return slot_header;

        slot_ref = slot_header->next;
    }
    return NULL;
}

template<typename T, typename T_Key, typename T_Compare>
bool MemRank<T, T_Key, T_Compare>::delete_node(MemRank<T, T_Key, T_Compare>::MRNode * node_)
{
    // 一定是从最顶层往下删
    assert(node_ != NULL);
    assert(node_->up == 0);

    // 跳跃表中包含对应节点的层中删除节点，修改对应后置节点的span
    size_t node_ref = ptr_2_ref(node_);
    size_t level_count = 0;
    while(node_ref != 0)
    {
        MRNode * node = ref_2_node(node_ref);
        MRNode * back_node = ref_2_node(node->back);
        MRNode * forward_node = ref_2_node(node->forward);
        back_node->forward = node->forward;
        forward_node->back = node->back;

        // 修改跳跃节点数
        back_node->span = back_node->span + node->span - 1;

        ++level_count;

        node_ref = node->down;
    }

    // 不包含对应节点的层中也要修改对应后置节点的span
    size_t next_ref = node_->back;
    while(next_ref != 0 && level_count < m_header->level_num)
    {
        MRNode * next_node = ref_2_node(next_ref);
        if(next_node->up != 0)
        {
            next_ref = next_node->up;
            next_node = ref_2_node(next_node->up);
            assert(next_node->span > 0);
            --(next_node->span);
            ++level_count;
        }
        else
        {
            next_ref = next_node->back;
        }
    }

    // 从hash中删除
    const KeyType & key = T_Key()(node_->info);
    size_t * pre = ref_2_uint(m_header->hash_head_ref + (key % m_header->bucket_num) * sizeof(size_t));
    size_t hash_ref = *pre;
    while(hash_ref != 0)
    {
        MRNode * hash_node = ref_2_node(hash_ref);

        if(hash_node == node_)
        {
            (*pre) = hash_node->next;
            break;
        }

        hash_ref = hash_node->next;
        pre = &(hash_node->next);
    }

    assert(hash_ref != 0);

    size_t delete_ref = ptr_2_ref(node_);
    while(delete_ref != 0)
    {
        MRNode * delete_node = ref_2_node(delete_ref);
        delete_ref = delete_node->down;
        free_node(delete_node);
    }

    --(m_header->t_num);

    return true;
}

template<typename T, typename T_Key, typename T_Compare>
typename MemRank<T, T_Key, T_Compare>::MRNode * MemRank<T, T_Key, T_Compare>::alloc_node()
{
    if(m_header->free_list == 0)
        return NULL;

    MRNode * p = ref_2_node(m_header->free_list);
    m_header->free_list = p->next;
    memset(p, 0, m_header->block_size);
    --(m_header->free_num);
    return p;
}

template<typename T, typename T_Key, typename T_Compare>
void MemRank<T, T_Key, T_Compare>::free_node(MRNode * node_)
{
    node_->next = m_header->free_list;
    m_header->free_list = ptr_2_ref(node_);
    ++(m_header->free_num);
}

}
#endif
