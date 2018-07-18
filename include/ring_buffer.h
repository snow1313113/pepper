/*
 * * file name: ring_buffer.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 02
 * */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <algorithm>
#include "head.h"
#include "utils/traits_utils.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE>
class FixedRingBuffer
{
public:
    /// 清空队列
    void Clear();
    /// 队列是否空
    bool IsEmpty() const;
    /// 队列是否满了
    bool IsFull() const;
    /// 当前已经用的个数
    size_t Size() const;
    /// 队列最大容量
    size_t Capacity() const;
    /// 队尾入队
    bool Push(const T & value_, bool over_write_ = false);
    /// 队头弹出一个
    void Pop();
    /// 获取队头第一个元素
    T & Front(size_t index_ = 0);
    /// 获取队头第一个元素
    const T & Front(size_t index_ = 0) const;
    /// 获取队尾第一个元素
    T & Back();
    /// 获取队尾第一个元素
    const T & Back() const;

private:
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;
    IntType m_start;
    IntType m_end;
    IntType m_used_len;
    T m_buf[MAX_SIZE];
};

template<typename T, size_t MAX_SIZE>
void FixedRingBuffer<T, MAX_SIZE>::Clear()
{
    m_start = 0;
    m_end = 0;
    m_used_len = 0;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuffer<T, MAX_SIZE>::IsEmpty() const
{
    return m_used_len == 0;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuffer<T, MAX_SIZE>::IsFull() const
{
    return m_used_len == MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
size_t FixedRingBuffer<T, MAX_SIZE>::Size() const
{
    return m_used_len;
}

template<typename T, size_t MAX_SIZE>
size_t FixedRingBuffer<T, MAX_SIZE>::Capacity() const
{
    return MAX_SIZE;
}

template<typename T, size_t MAX_SIZE>
bool FixedRingBuffer<T, MAX_SIZE>::Push(const T & value_, bool over_write_)
{
    if (over_write_)
    {
        if (IsFull())
            Pop();
    }
    else
    {
        if (IsFull())
            return false;
    }

    m_buf[m_end] = value_;
    m_end = ((m_end + 1) % MAX_SIZE);
    ++m_used_len;

    return true;
}

template<typename T, size_t MAX_SIZE>
void FixedRingBuffer<T, MAX_SIZE>::Pop()
{
    if (IsEmpty() == false)
    {
        m_start = ((m_start + 1) % MAX_SIZE);
        --m_used_len;
    }
}

template<typename T, size_t MAX_SIZE>
T & FixedRingBuffer<T, MAX_SIZE>::Front(size_t index_)
{
    assert(index_ < m_used_len);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
const T & FixedRingBuffer<T, MAX_SIZE>::Front(size_t index_) const
{
    assert(index_ < m_used_len);
    return m_buf[(m_start + index_) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
T & FixedRingBuffer<T, MAX_SIZE>::Back()
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}

template<typename T, size_t MAX_SIZE>
const T & FixedRingBuffer<T, MAX_SIZE>::Back() const
{
    return m_buf[(m_end + MAX_SIZE - 1) % MAX_SIZE];
}
 
template<size_t MAX_SIZE>
class UnfixedRingBuffer
{
public:
    /// 清空队列
    void Clear();
    /// 队列是否空
    bool IsEmpty() const;
    /// 队列是否满了
    bool IsFull() const;
    /// 当前已经使用的字节数
    size_t Size() const;
    /// 队列最大字节数容量
    size_t Capacity() const;
    /// 获取插入了多少数据包
    size_t GetNum() const;
    /// 队尾入队
    bool Push(const uint8_t * data_, size_t len_, bool over_write_ = false);
    /// 队头弹出一个
    void Pop();
    /// 获取队头往后数第index_个元素(从0开始计数)，返回该元素的指针，len_表示数据长度
    uint8_t * Front(size_t & len_, size_t index_ = 0);

private:
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;

    bool PushImpl(const uint8_t * data_, size_t len_, size_t need_len_, bool over_write_);
    /// 在尾部补一个节点到补满
    void PushPaddingItem();
    /// 把填充用的节点弹出
    void PopPaddingItem();
    /// 跳过buffer尾部不满ItemHeader大小的字节
    IntType NeedSkipBytes(IntType cur_pos_);

private:
    struct ItemHeader
    {
        /// 0 表示数据节点，1 表示填充用的
        IntType m_flag;
        /// 后面的数据长度
        IntType m_len;
    };

    IntType m_start;
    IntType m_end;
    IntType m_used_size;
    IntType m_item_num;
    /// todo 其实可以把max_size做一下对齐的，后面再加吧
    uint8_t m_buf[MAX_SIZE];
};

template<size_t MAX_SIZE>
void UnfixedRingBuffer<MAX_SIZE>::Clear()
{
    m_start = 0;
    m_end = 0;
    m_used_size = 0;
    m_item_num = 0;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuffer<MAX_SIZE>::IsEmpty() const
{
    return m_used_size == 0;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuffer<MAX_SIZE>::IsFull() const
{
    return m_used_size >= MAX_SIZE;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuffer<MAX_SIZE>::Size() const
{
    return m_used_size;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuffer<MAX_SIZE>::Capacity() const
{
    return MAX_SIZE;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuffer<MAX_SIZE>::GetNum() const
{
    return m_item_num;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuffer<MAX_SIZE>::Push(const uint8_t * data_, size_t len_, bool over_write_)
{
    size_t need_len = len_ + sizeof(ItemHeader);
    if (need_len > MAX_SIZE)
        return false;
    return PushImpl(data_, len_, need_len, over_write_);
}

template<size_t MAX_SIZE>
void UnfixedRingBuffer<MAX_SIZE>::Pop()
{
    if (IsEmpty())
        return;

    // 每次pop的时候都会把后面的padding Item或者小空隙pop完
    // 所以每次到这里的时候一定不可能没有一个完整的Item
    assert(m_used_size >= sizeof(ItemHeader));
    ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + m_start);

    assert(item_header->m_flag == 0);
    m_start = (m_start + sizeof(ItemHeader) + item_header->m_len) % MAX_SIZE;

    assert(m_used_size >= (sizeof(ItemHeader) + item_header->m_len));
    m_used_size -= (sizeof(ItemHeader) + item_header->m_len);

    assert(m_item_num > 0);
    --m_item_num;

    IntType skip_bytes = NeedSkipBytes(m_start);
    if (skip_bytes > 0)
    {
        m_start = (m_start + skip_bytes) % MAX_SIZE;
        assert(m_used_size >= skip_bytes);
        m_used_size -= skip_bytes;
    }
    else
    {
        if (!IsEmpty())
        {
            PopPaddingItem();
        }
    }

    if (IsEmpty())
    {
        // 当是空的时候，修正一下start和end的位置
        // 对于overwrite == false的情况不会有出现buffer被切成两段的情况
        m_start = 0;
        m_end = 0;
    }
}

template<size_t MAX_SIZE>
uint8_t * UnfixedRingBuffer<MAX_SIZE>::Front(size_t & len_, size_t index_)
{
    if (IsEmpty())
        return NULL;

    IntType item_start = m_start;
    for (size_t i = 0; i < index_; ++i)
    {
        ItemHeader * header = reinterpret_cast<ItemHeader *>(m_buf + item_start);
        item_start = (item_start + sizeof(ItemHeader) + header->m_len) % MAX_SIZE;

        IntType skip_bytes = NeedSkipBytes(item_start);
        if (skip_bytes > 0)
        {
            item_start = (item_start + skip_bytes) % MAX_SIZE;
        }
        else
        {
            if (item_start != m_end)
            {
                ItemHeader * padding_header = reinterpret_cast<ItemHeader *>(m_buf + item_start);
                if (padding_header->m_flag == 1)
                    item_start = (item_start + sizeof(ItemHeader) + padding_header->m_len) % MAX_SIZE;
            }
        }

        if (item_start == m_end)
            return NULL;
    }

    ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<uint8_t*>(item_header + 1);
}

template<size_t MAX_SIZE>
bool UnfixedRingBuffer<MAX_SIZE>::PushImpl(const uint8_t * data_, size_t len_, size_t need_len_, bool over_write_)
{
    if (IsFull())
    {
        if (!over_write_)
            return false;
        else
            Pop();
    }

    // 不会是满的，所以如果m_end == m_start只能是空的
    if (m_end >= m_start)
    {
        // 尾部能插入
        if (m_end + need_len_ <= MAX_SIZE)
        {
            ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + m_end);
            item_header->m_len = len_;
            item_header->m_flag = 0;
            std::copy_n(data_, len_, m_buf + m_end + sizeof(ItemHeader));
            m_end = (m_end + need_len_) % MAX_SIZE;
            m_used_size += need_len_;
            ++m_item_num;

            IntType skip_bytes = NeedSkipBytes(m_end);
            m_end = (m_end + skip_bytes) % MAX_SIZE;
            m_used_size += skip_bytes;

            return true;
        }

        // 尾部空间不够，看循环过去start的前面空间够不够
        if (!over_write_ && m_start < need_len_)
            return false;

        PushPaddingItem();
        if (over_write_)
        {
            while(!IsEmpty() && m_start < need_len_)
                Pop();
        }

        return PushImpl(data_, len_, need_len_, over_write_);
    }
    else
    {
        // 尾部能插入
        if (m_end + need_len_ <= m_start)
        {
            ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + m_end);
            item_header->m_len = len_;
            item_header->m_flag = 0;
            std::copy_n(data_, len_, m_buf + m_end + sizeof(ItemHeader));
            m_end += need_len_;
            m_used_size += need_len_;
            ++m_item_num;

            return true;
        }

        // 不能覆盖，那一定放不下了
        if (!over_write_)
            return false;

        if (m_end + need_len_ <= MAX_SIZE)
        {
            while(m_end < m_start && m_end + need_len_ > m_start)
                Pop();
            assert(m_start == 0 || m_end + need_len_ <= m_start);
            return PushImpl(data_, len_, need_len_, over_write_);
        }

        do
        {
            Pop();
        }
        while(m_end < m_start);

        PushPaddingItem();

        assert(IsFull());
        return PushImpl(data_, len_, need_len_, over_write_);
    }

    return false;
}

template<size_t MAX_SIZE>
void UnfixedRingBuffer<MAX_SIZE>::PushPaddingItem()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(m_start <= m_end);
    ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + m_end);
    item_header->m_len = (MAX_SIZE - m_end - sizeof(ItemHeader));
    item_header->m_flag = 1;
    m_used_size += (MAX_SIZE - m_end);
    m_end = 0;
}

template<size_t MAX_SIZE>
void UnfixedRingBuffer<MAX_SIZE>::PopPaddingItem()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(m_used_size >= sizeof(ItemHeader));
    ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + m_start);
    if (item_header->m_flag == 1)
    {
        m_start = (m_start + sizeof(ItemHeader) + item_header->m_len) % MAX_SIZE;
        m_used_size -= (sizeof(ItemHeader) + item_header->m_len);
        assert(m_used_size >= 0);
    }
}

template<size_t MAX_SIZE>
typename UnfixedRingBuffer<MAX_SIZE>::IntType UnfixedRingBuffer<MAX_SIZE>::NeedSkipBytes(UnfixedRingBuffer<MAX_SIZE>::IntType cur_pos_)
{
    assert(cur_pos_ <= MAX_SIZE);
    if (cur_pos_ + sizeof(ItemHeader) > MAX_SIZE)
        return MAX_SIZE - cur_pos_;
    else
        return 0;
}

}

#endif
