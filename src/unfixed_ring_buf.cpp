/*
 * * file name: unfixed_ring_buf.cpp
 * * description: ...
 * * author: snow
 * * create time:2019  6 14
 * */

#include "unfixed_ring_buf.h"

namespace Pepper
{

bool UnfixedRingBuf<0>::init(void *mem_, size_t mem_size_, bool check_)
{
    if (!mem_ || mem_size_ < sizeof(BuffHead))
        return false;

    m_head = reinterpret_cast<BuffHead *>(mem_);
    if (check_)
    {
        if (m_head->m_size != (mem_size_ - sizeof(BuffHead)) || m_head->m_used_size > m_head->m_size)
            return false;
    }
    else
    {
        m_head->m_start = 0;
        m_head->m_end = 0;
        m_head->m_used_size = 0;
        m_head->m_item_num = 0;
        m_head->m_size = mem_size_ - sizeof(BuffHead);
    }

    m_buf = reinterpret_cast<uint8_t *>(mem_) + sizeof(BuffHead);
    return true;
}

void UnfixedRingBuf<0>::clear()
{
    m_head->m_start = 0;
    m_head->m_end = 0;
    m_head->m_used_size = 0;
    m_head->m_item_num = 0;
}

bool UnfixedRingBuf<0>::empty() const
{
    return m_head->m_used_size == 0 && m_head->m_used_size < m_head->m_size;
}

bool UnfixedRingBuf<0>::full() const
{
    return m_head->m_used_size >= m_head->m_size;
}

size_t UnfixedRingBuf<0>::size() const
{
    return m_head->m_used_size;
}

size_t UnfixedRingBuf<0>::capacity() const
{
    return m_head->m_size;
}

size_t UnfixedRingBuf<0>::get_num() const
{
    return m_head->m_item_num;
}

bool UnfixedRingBuf<0>::push(const uint8_t *data_, size_t len_, bool over_write_)
{
    struct iovec iov[1];
    iov[0].iov_base = const_cast<void *>(reinterpret_cast<const void *>(data_));
    iov[0].iov_len = len_;
    return push(iov, 1, over_write_);
}

bool UnfixedRingBuf<0>::push(const struct iovec *iov_, size_t iov_cnt_, bool over_write_)
{
    size_t total_len = 0;
    for (size_t i = 0; i < iov_cnt_; ++i)
        total_len += iov_[i].iov_len;

    size_t need_len = total_len + sizeof(ItemHeader);
    if (need_len > m_head->m_size)
        return false;
    return push_impl(iov_, iov_cnt_, total_len, need_len, over_write_);
}

void UnfixedRingBuf<0>::pop()
{
    if (empty())
        return;

    // 每次pop的时候都会把后面的padding Item或者小空隙pop完
    // 所以每次到这里的时候一定不可能没有一个完整的Item
    assert(m_head->m_used_size >= sizeof(ItemHeader));
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + m_head->m_start);

    assert(item_header->m_flag == 0);
    m_head->m_start = (m_head->m_start + sizeof(ItemHeader) + item_header->m_len) % m_head->m_size;

    assert(m_head->m_used_size >= (sizeof(ItemHeader) + item_header->m_len));
    m_head->m_used_size -= (sizeof(ItemHeader) + item_header->m_len);

    assert(m_head->m_item_num > 0);
    --(m_head->m_item_num);

    IntType skip_bytes = need_skip_bytes(m_head->m_start);
    if (skip_bytes > 0)
    {
        m_head->m_start = (m_head->m_start + skip_bytes) % m_head->m_size;
        assert(m_head->m_used_size >= skip_bytes);
        m_head->m_used_size -= skip_bytes;
    }
    else
    {
        if (!empty())
        {
            pop_padding();
        }
    }

    if (empty())
    {
        // 当是空的时候，修正一下start和end的位置
        // 对于overwrite == false的情况不会有出现buffer被切成两段的情况
        m_head->m_start = 0;
        m_head->m_end = 0;
    }
}

typename UnfixedRingBuf<0>::IntType UnfixedRingBuf<0>::find_start(size_t index_) const
{
    IntType item_start = m_head->m_start;
    for (size_t i = 0; i < index_; ++i)
    {
        const ItemHeader *header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
        item_start = (item_start + sizeof(ItemHeader) + header->m_len) % m_head->m_size;

        IntType skip_bytes = need_skip_bytes(item_start);
        if (skip_bytes > 0)
        {
            item_start = (item_start + skip_bytes) % m_head->m_size;
        }
        else
        {
            if (item_start != m_head->m_end)
            {
                const ItemHeader *padding_header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
                if (padding_header->m_flag == 1)
                    item_start = (item_start + sizeof(ItemHeader) + padding_header->m_len) % m_head->m_size;
            }
        }

        if (item_start == m_head->m_end)
            break;
    }

    return item_start;
}

const uint8_t *UnfixedRingBuf<0>::front(size_t &len_, size_t index_) const
{
    if (empty())
        return NULL;

    IntType item_start = m_head->m_start;
    if (index_ > 0)
    {
        // 如果不是第一个节点，则找开始的偏移，找到尾部为止
        item_start = find_start(index_);
        if (item_start == m_head->m_end)
            return NULL;
    }

    const ItemHeader *item_header = reinterpret_cast<const ItemHeader *>(m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<const uint8_t *>(item_header + 1);
}

uint8_t *UnfixedRingBuf<0>::front(size_t &len_, size_t index_)
{
    if (empty())
        return NULL;

    IntType item_start = m_head->m_start;
    if (index_ > 0)
    {
        // 如果不是第一个节点，则找开始的偏移，找到尾部为止
        item_start = find_start(index_);
        if (item_start == m_head->m_end)
            return NULL;
    }

    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<uint8_t *>(item_header + 1);
}

bool UnfixedRingBuf<0>::push_impl(const struct iovec *iov_, size_t iov_cnt_, size_t total_len_, size_t need_len_,
                                    bool over_write_)
{
    if (full())
    {
        if (!over_write_)
            return false;
        else
            pop();
    }

    // 不会是满的，所以如果m_end == m_start只能是空的
    if (m_head->m_end >= m_head->m_start)
    {
        // 尾部能插入
        if (m_head->m_end + need_len_ <= m_head->m_size)
        {
            ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + m_head->m_end);
            item_header->m_len = total_len_;
            item_header->m_flag = 0;

            uint8_t *begin = m_buf + m_head->m_end + sizeof(ItemHeader);
            for (size_t i = 0; i < iov_cnt_; ++i)
            {
                std::memcpy(begin, iov_[i].iov_base, iov_[i].iov_len);
                begin += iov_[i].iov_len;
            }

            m_head->m_end = (m_head->m_end + need_len_) % m_head->m_size;
            m_head->m_used_size += need_len_;
            ++(m_head->m_item_num);

            IntType skip_bytes = need_skip_bytes(m_head->m_end);
            m_head->m_end = (m_head->m_end + skip_bytes) % m_head->m_size;
            m_head->m_used_size += skip_bytes;

            return true;
        }

        // 尾部空间不够，看循环过去start的前面空间够不够
        if (!over_write_ && m_head->m_start < need_len_)
            return false;

        push_padding();
        if (over_write_)
        {
            while (!empty() && m_head->m_start < need_len_)
                pop();
        }

        return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
    }
    else
    {
        // 尾部能插入
        if (m_head->m_end + need_len_ <= m_head->m_start)
        {
            ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + m_head->m_end);
            item_header->m_len = total_len_;
            item_header->m_flag = 0;

            uint8_t *begin = m_buf + m_head->m_end + sizeof(ItemHeader);
            for (size_t i = 0; i < iov_cnt_; ++i)
            {
                std::memcpy(begin, iov_[i].iov_base, iov_[i].iov_len);
                begin += iov_[i].iov_len;
            }

            m_head->m_end += need_len_;
            m_head->m_used_size += need_len_;
            ++(m_head->m_item_num);

            return true;
        }

        // 不能覆盖，那一定放不下了
        if (!over_write_)
            return false;

        if (m_head->m_end + need_len_ <= m_head->m_size)
        {
            while (m_head->m_end < m_head->m_start && m_head->m_end + need_len_ > m_head->m_start)
                pop();
            assert(m_head->m_start == 0 || m_head->m_end + need_len_ <= m_head->m_start);
            return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
        }

        do
        {
            pop();
        } while (m_head->m_end < m_head->m_start);

        push_padding();

        assert(full());
        return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
    }

    return false;
}

void UnfixedRingBuf<0>::push_padding()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(m_head->m_start <= m_head->m_end);
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + m_head->m_end);
    item_header->m_len = (m_head->m_size - m_head->m_end - sizeof(ItemHeader));
    item_header->m_flag = 1;
    m_head->m_used_size += (m_head->m_size - m_head->m_end);
    m_head->m_end = 0;
}

void UnfixedRingBuf<0>::pop_padding()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(m_head->m_used_size >= sizeof(ItemHeader));
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(m_buf + m_head->m_start);
    if (item_header->m_flag == 1)
    {
        m_head->m_start = (m_head->m_start + sizeof(ItemHeader) + item_header->m_len) % m_head->m_size;
        m_head->m_used_size -= (sizeof(ItemHeader) + item_header->m_len);
        assert(m_head->m_used_size >= 0);
    }
}

typename UnfixedRingBuf<0>::IntType UnfixedRingBuf<0>::need_skip_bytes(UnfixedRingBuf<0>::IntType cur_pos_) const
{
    assert(cur_pos_ <= m_head->m_size);
    if (cur_pos_ + sizeof(ItemHeader) > m_head->m_size)
        return m_head->m_size - cur_pos_;
    else
        return 0;
}

}
