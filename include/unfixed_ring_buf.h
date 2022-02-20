/*
 * * file name: unfixed_ring_buf.h
 * * description: ...
 * * author: snow
 * * create time:2019  5 11
 * */

#ifndef _UNFIXED_RING_BUF_H_
#define _UNFIXED_RING_BUF_H_

#include <sys/uio.h>
#include "inner/buf_data.h"

namespace pepper
{
/// SIZE 如果是0，则表示大小是通过init来指定
template <size_t MAX_SIZE = 0>
class UnfixedRingBuf : public UnfixedRingBufData<MAX_SIZE>
{
public:
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
    size_t get_num() const;
    /// 队尾入队
    bool push(const uint8_t *data_, size_t len_, bool over_write_ = false);
    bool push(const struct iovec *iov_, size_t iov_cnt_, bool over_write_ = false);
    /// 队头弹出一个
    void pop();
    /// 获取队头往后数第index_个元素(从0开始计数)，返回该元素的指针，len_表示数据长度
    const uint8_t *front(size_t &len_, size_t index_ = 0) const;
    uint8_t *front(size_t &len_, size_t index_ = 0);

private:
    using Data = UnfixedRingBufData<MAX_SIZE>;
    using IntType = typename Data::IntType;

    struct ItemHeader
    {
        /// 0 表示数据节点，1 表示填充用的
        uint8_t m_flag = 0;
        /// 后面的数据长度
        IntType m_len = 0;
    };

    /// 找不到就返回MAX_SIZE
    IntType find_start(size_t index_) const;
    bool push_impl(const struct iovec *iov_, size_t iov_cnt_, size_t total_len_, size_t need_len_, bool over_write_);
    /// 在尾部补一个节点到补满
    void push_padding();
    /// 把填充用的节点弹出
    void pop_padding();
    /// 跳过buffer尾部不满ItemHeader大小的字节
    IntType need_skip_bytes(IntType cur_pos_) const;
};

template <size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::clear()
{
    Data::set_start(0);
    Data::set_end(0);
    Data::set_used_size(0);
    Data::set_item_num(0);
}

template <size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::empty() const
{
    return Data::get_used_size() == 0 && sizeof(ItemHeader) < Data::get_max_size();
}

template <size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::full() const
{
    // todo 这里需要考虑padding的情况
    return Data::get_used_size() >= Data::get_max_size();
}

template <size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::size() const
{
    return Data::get_used_size();
}

template <size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::capacity() const
{
    return Data::get_max_size();
}

template <size_t MAX_SIZE>
size_t UnfixedRingBuf<MAX_SIZE>::get_num() const
{
    return Data::get_item_num();
}

template <size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::push(const uint8_t *data_, size_t len_, bool over_write_)
{
    struct iovec iov[1];
    iov[0].iov_base = const_cast<void *>(reinterpret_cast<const void *>(data_));
    iov[0].iov_len = len_;
    return push(iov, 1, over_write_);
}

template <size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::push(const struct iovec *iov_, size_t iov_cnt_, bool over_write_)
{
    size_t total_len = 0;
    for (size_t i = 0; i < iov_cnt_; ++i)
        total_len += iov_[i].iov_len;

    size_t need_len = total_len + sizeof(ItemHeader);
    if (need_len > Data::get_max_size())
        return false;
    return push_impl(iov_, iov_cnt_, total_len, need_len, over_write_);
}

template <size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::pop()
{
    if (empty())
        return;

    // 每次pop的时候都会把后面的padding Item或者小空隙pop完
    // 所以每次到这里的时候一定不可能没有一个完整的Item
    assert(Data::get_used_size() >= sizeof(ItemHeader));
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + Data::get_start());

    assert(item_header->m_flag == 0);
    Data::set_start((Data::get_start() + sizeof(ItemHeader) + item_header->m_len) % Data::get_max_size());

    assert(Data::get_used_size() >= (sizeof(ItemHeader) + item_header->m_len));
    Data::decr_used_size(sizeof(ItemHeader) + item_header->m_len);

    assert(Data::get_item_num() > 0);
    Data::decr_item_num();

    IntType skip_bytes = need_skip_bytes(Data::get_start());
    if (skip_bytes > 0)
    {
        Data::set_start((Data::get_start() + skip_bytes) % Data::get_max_size());
        assert(Data::get_used_size() >= skip_bytes);
        Data::decr_used_size(skip_bytes);
    }
    else
    {
        if (!empty())
            pop_padding();
    }

    if (empty())
    {
        // 当是空的时候，修正一下start和end的位置
        // 对于overwrite == false的情况不会有出现buffer被切成两段的情况
        Data::set_start(0);
        Data::set_end(0);
    }
}

template <size_t MAX_SIZE>
const uint8_t *UnfixedRingBuf<MAX_SIZE>::front(size_t &len_, size_t index_) const
{
    IntType item_start = find_start(index_);
    if (item_start == Data::get_max_size())
        return nullptr;

    const ItemHeader *item_header = reinterpret_cast<const ItemHeader *>(Data::m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<const uint8_t *>(item_header + 1);
}

template <size_t MAX_SIZE>
uint8_t *UnfixedRingBuf<MAX_SIZE>::front(size_t &len_, size_t index_)
{
    IntType item_start = find_start(index_);
    if (item_start == Data::get_max_size())
        return nullptr;

    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + item_start);
    len_ = item_header->m_len;
    return reinterpret_cast<uint8_t *>(item_header + 1);
}

template <size_t MAX_SIZE>
typename UnfixedRingBuf<MAX_SIZE>::IntType UnfixedRingBuf<MAX_SIZE>::find_start(size_t index_) const
{
    if (empty() || index_ >= Data::get_item_num())
        return Data::get_max_size();

    if (index_ == 0)
        return Data::get_start();

    IntType item_start = Data::get_start();
    for (size_t i = 0; i < index_; ++i)
    {
        const ItemHeader *header = reinterpret_cast<const ItemHeader *>(Data::m_buf + item_start);
        item_start = (item_start + sizeof(ItemHeader) + header->m_len) % Data::get_max_size();

        IntType skip_bytes = need_skip_bytes(item_start);
        if (skip_bytes > 0)
            item_start = (item_start + skip_bytes) % Data::get_max_size();
        else
        {
            if (item_start != Data::get_end())
            {
                const ItemHeader *padding_header = reinterpret_cast<const ItemHeader *>(Data::m_buf + item_start);
                if (padding_header->m_flag == 1)
                    item_start = (item_start + sizeof(ItemHeader) + padding_header->m_len) % Data::get_max_size();
            }
        }

        // 找了一圈都找不到，理论上不应该
        if (item_start == Data::get_end())
            return Data::get_max_size();
    }
    return item_start;
}

template <size_t MAX_SIZE>
bool UnfixedRingBuf<MAX_SIZE>::push_impl(const struct iovec *iov_, size_t iov_cnt_, size_t total_len_, size_t need_len_,
                                         bool over_write_)
{
    if (full())
    {
        if (!over_write_)
            return false;
        else
            pop();
    }

    // 不会是满的，所以如果end == start只能是空的
    if (Data::get_end() >= Data::get_start())
    {
        // 尾部能插入
        if (Data::get_end() + need_len_ <= Data::get_max_size())
        {
            ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + Data::get_end());
            item_header->m_len = total_len_;
            item_header->m_flag = 0;

            uint8_t *begin = Data::m_buf + Data::get_end() + sizeof(ItemHeader);
            for (size_t i = 0; i < iov_cnt_; ++i)
            {
                std::memcpy(begin, iov_[i].iov_base, iov_[i].iov_len);
                begin += iov_[i].iov_len;
            }

            Data::set_end((Data::get_end() + need_len_) % Data::get_max_size());
            Data::incr_used_size(need_len_);
            Data::incr_item_num();

            IntType skip_bytes = need_skip_bytes(Data::get_end());
            Data::set_end((Data::get_end() + skip_bytes) % Data::get_max_size());
            Data::incr_used_size(skip_bytes);
            return true;
        }

        // 尾部空间不够，看循环过去start的前面空间够不够
        if (!over_write_ && Data::get_start() < need_len_)
            return false;

        push_padding();
        if (over_write_)
        {
            while (!empty() && Data::get_start() < need_len_)
                pop();
        }

        return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
    }
    else
    {
        // 尾部能插入
        if (Data::get_end() + need_len_ <= Data::get_start())
        {
            ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + Data::get_end());
            item_header->m_len = total_len_;
            item_header->m_flag = 0;

            uint8_t *begin = Data::m_buf + Data::get_end() + sizeof(ItemHeader);
            for (size_t i = 0; i < iov_cnt_; ++i)
            {
                std::memcpy(begin, iov_[i].iov_base, iov_[i].iov_len);
                begin += iov_[i].iov_len;
            }

            Data::set_end(Data::get_end() + need_len_);
            Data::incr_used_size(need_len_);
            Data::incr_item_num();
            return true;
        }

        // 不能覆盖，那一定放不下了
        if (!over_write_)
            return false;

        if (Data::get_end() + need_len_ <= Data::get_max_size())
        {
            while (Data::get_end() < Data::get_start() && Data::get_end() + need_len_ > Data::get_start())
                pop();
            assert(Data::get_start() == 0 || Data::get_end() + need_len_ <= Data::get_start());
            return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
        }

        do
        {
            pop();
        } while (Data::get_end() < Data::get_start());

        push_padding();

        assert(full());
        return push_impl(iov_, iov_cnt_, total_len_, need_len_, over_write_);
    }

    return false;
}

template <size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::push_padding()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(Data::get_start() <= Data::get_end());
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + Data::get_end());
    item_header->m_len = (Data::get_max_size() - Data::get_end() - sizeof(ItemHeader));
    item_header->m_flag = 1;
    Data::incr_used_size(Data::get_max_size() - Data::get_end());
    Data::set_end(0);
}

template <size_t MAX_SIZE>
void UnfixedRingBuf<MAX_SIZE>::pop_padding()
{
    // 如果，一个ItemHeader都放不下呢，在push和pop的时候
    // 都会跳过尾部不足一个ItemHeader大小的字节
    assert(Data::get_used_size() >= sizeof(ItemHeader));
    ItemHeader *item_header = reinterpret_cast<ItemHeader *>(Data::m_buf + Data::get_start());
    if (item_header->m_flag == 1)
    {
        Data::set_start((Data::get_start() + sizeof(ItemHeader) + item_header->m_len) % Data::get_max_size());
        Data::decr_used_size(sizeof(ItemHeader) + item_header->m_len);
        assert(Data::get_used_size() >= 0);
    }
}

template <size_t MAX_SIZE>
typename UnfixedRingBuf<MAX_SIZE>::IntType UnfixedRingBuf<MAX_SIZE>::need_skip_bytes(
    UnfixedRingBuf<MAX_SIZE>::IntType cur_pos_) const
{
    assert(cur_pos_ <= Data::get_max_size());
    if (cur_pos_ + sizeof(ItemHeader) > Data::get_max_size())
        return Data::get_max_size() - cur_pos_;
    else
        return 0;
}
}  // namespace pepper

#endif
