/*
 * * file name: unfixed_ring_buf.h
 * * description: ...
 * * author: snow
 * * create time:2019  5 11
 * */

#ifndef _UNFIXED_RING_BUF_H_
#define _UNFIXED_RING_BUF_H_

#include <algorithm>
#include "inner/head.h"
#include "utils/traits_utils.h"

namespace Pepper
{

template<size_t MAX_SIZE>
class UnfixedRingBuf
{
public:
    constexpr UnfixedRingBuf() noexcept = default;
    /// 清空队列
    void clear();
    /// 队列是否空
    bool empty() const;
    /// 队列是否满了
    bool full() const;
    /// 当前已经使用的字节数
    size_t size() const;
    /// 队列最大字节数容量
    size_t capacity() const;
    /// 获取插入了多少数据包
    size_t GetNum() const;
    /// 队尾入队
    bool push(const uint8_t * data_, size_t len_, bool over_write_ = false);
    /// 队头弹出一个
    void pop();
    /// 获取队头往后数第index_个元素(从0开始计数)，返回该元素的指针，len_表示数据长度
    const uint8_t * front(size_t & len_, size_t index_ = 0) const;
    uint8_t * front(size_t & len_, size_t index_ = 0);

private:
    typedef typename FixIntType<MAX_SIZE>::IntType IntType;

    IntType FindStartOffset(size_t index_) const;
    bool PushImpl(const uint8_t * data_, size_t len_, size_t need_len_, bool over_write_);
    /// 在尾部补一个节点到补满
    void PushPaddingItem();
    /// 把填充用的节点弹出
    void PopPaddingItem();
    /// 跳过buffer尾部不满ItemHeader大小的字节
    IntType NeedSkipBytes(IntType cur_pos_) const;

private:
    struct ItemHeader
    {
        /// 0 表示数据节点，1 表示填充用的
        uint8_t m_flag;
        /// 后面的数据长度
        IntType m_len;
    };

    IntType m_start = 0;
    IntType m_end = 0;
    IntType m_used_size = 0;
    IntType m_item_num = 0;
    /// todo 其实可以把max_size做一下对齐的，后面再加吧
    uint8_t m_buf[MAX_SIZE];
};

template<size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::clear()
{
    m_start = 0;
    m_end = 0;
    m_used_size = 0;
    m_item_num = 0;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::empty() const
{
    return m_used_size == 0;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::full() const
{
    return m_used_size >= MAX_SIZE;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::size() const
{
    return m_used_size;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::capacity() const
{
    return MAX_SIZE;
}

template<size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::GetNum() const
{
    return m_item_num;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::push(const uint8_t * data_, size_t len_, bool over_write_)
{
    size_t need_len = len_ + sizeof(ItemHeader);
    if (need_len > MAX_SIZE)
        return false;
    return PushImpl(data_, len_, need_len, over_write_);
}

template<size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::pop()
{
    if (empty())
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
        if (!empty())
        {
            PopPaddingItem();
        }
    }

    if (empty())
    {
        // 当是空的时候，修正一下start和end的位置
        // 对于overwrite == false的情况不会有出现buffer被切成两段的情况
        m_start = 0;
        m_end = 0;
    }
}

template<size_t MAX_SIZE>
const uint8_t * UnfixedRingBuf<MAX_SIZE>::front(size_t & len_, size_t index_) const
{
    if (empty())
        return NULL;

    IntType item_start = FindStartOffset(index_);
    if (item_start == m_end)
        return NULL;
 
    const ItemHeader * item_header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<const uint8_t*>(item_header + 1);
}

template<size_t MAX_SIZE>
uint8_t * UnfixedRingBuf<MAX_SIZE>::front(size_t & len_, size_t index_)
{
    if (empty())
        return NULL;

    IntType item_start = FindStartOffset(index_);
    if (item_start == m_end)
        return NULL;
 
    ItemHeader * item_header = reinterpret_cast<ItemHeader *>(m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<uint8_t*>(item_header + 1);
}

template<size_t MAX_SIZE>
typename UnfixedRingBuf<MAX_SIZE>::IntType UnfixedRingBuf<MAX_SIZE>::FindStartOffset(size_t index_) const
{
    IntType item_start = m_start;
    for (size_t i = 0; i < index_; ++i)
    {
        const ItemHeader * header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
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
                const ItemHeader * padding_header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
                if (padding_header->m_flag == 1)
                    item_start = (item_start + sizeof(ItemHeader) + padding_header->m_len) % MAX_SIZE;
            }
        }

        if (item_start == m_end)
            break;
    }
    return item_start;
}

template<size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::PushImpl(const uint8_t * data_, size_t len_, size_t need_len_, bool over_write_)
{
    if (full())
    {
        if (!over_write_)
            return false;
        else
            pop();
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
            while(!empty() && m_start < need_len_)
                pop();
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
                pop();
            assert(m_start == 0 || m_end + need_len_ <= m_start);
            return PushImpl(data_, len_, need_len_, over_write_);
        }

        do
        {
            pop();
        }
        while(m_end < m_start);

        PushPaddingItem();

        assert(full());
        return PushImpl(data_, len_, need_len_, over_write_);
    }

    return false;
}

template<size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::PushPaddingItem()
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
void UnfixedRingBuf<MAX_SIZE>::PopPaddingItem()
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
typename UnfixedRingBuf<MAX_SIZE>::IntType UnfixedRingBuf<MAX_SIZE>::NeedSkipBytes(UnfixedRingBuf<MAX_SIZE>::IntType cur_pos_) const
{
    assert(cur_pos_ <= MAX_SIZE);
    if (cur_pos_ + sizeof(ItemHeader) > MAX_SIZE)
        return MAX_SIZE - cur_pos_;
    else
        return 0;
}

#endif
