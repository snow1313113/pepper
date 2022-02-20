/*
 * * file name: buf_data.h
 * * description: ...
 * * author: snow
 * * create time:2022  2 19
 * */

#ifndef _BUF_DATA_H_
#define _BUF_DATA_H_

#include <algorithm>
#include <type_traits>
#include "inner/head.h"
#include "utils/traits_utils.h"

namespace pepper
{
namespace inner
{
template <typename T, size_t MAX_SIZE = 0, typename = void>
struct FixedRingBufData;

template <typename T, size_t MAX_SIZE>
struct FixedRingBufData<T, MAX_SIZE, std::enable_if_t<std::is_trivially_copyable_v<T>>>
{
protected:
    using IntType = typename FixIntType<MAX_SIZE>::IntType;

    IntType constexpr get_start() const { return m_start; }
    IntType constexpr get_end() const { return m_end; }
    IntType constexpr get_used_num() const { return m_used_num; }
    IntType constexpr get_max_num() const { return MAX_SIZE; }

    inline void set_start(IntType start_) { m_start = start_; }
    inline void set_end(IntType end_) { m_end = end_; }
    inline void set_used_num(IntType used_num_) { m_used_num = used_num_; }
    inline void incr_used_num() { ++m_used_num; }
    inline void decr_used_num() { --m_used_num; }

    IntType m_start = 0;
    IntType m_end = 0;
    IntType m_used_num = 0;
    T m_buf[MAX_SIZE];
};

template <typename T>
struct FixedRingBufData<T, 0, std::enable_if_t<std::is_trivially_copyable_v<T>>>
{
protected:
    using IntType = size_t;

    IntType constexpr get_start() const { return m_head->m_start; }
    IntType constexpr get_end() const { return m_head->m_end; }
    IntType constexpr get_used_num() const { return m_head->m_used_num; }
    IntType constexpr get_max_num() const { return m_head->m_max_num; }

    inline void set_start(IntType start_) { m_head->m_start = start_; }
    inline void set_end(IntType end_) { m_head->m_end = end_; }
    inline void set_used_num(IntType used_num_) { m_head->m_used_num = used_num_; }
    inline void incr_used_num() { ++(m_head->m_used_num); }
    inline void decr_used_num() { --(m_head->m_used_num); }

    struct BuffHead
    {
        IntType m_start = 0;
        IntType m_end = 0;
        IntType m_used_num = 0;
        IntType m_max_num = 0;
    };
    BuffHead *m_head = nullptr;
    T *m_buf = nullptr;

public:
    static size_t need_mem_size(size_t size_) { return sizeof(BuffHead) + sizeof(T) * size_; }

    bool init(void *mem_, size_t mem_size_, bool check_ = false)
    {
        if (!mem_ || mem_size_ < sizeof(BuffHead))
            return false;

        m_head = reinterpret_cast<BuffHead *>(mem_);
        if (check_)
        {
            if (m_head->m_max_num != (mem_size_ - sizeof(BuffHead)) / sizeof(T))
                return false;
        }
        else
        {
            m_head->m_start = 0;
            m_head->m_end = 0;
            m_head->m_used_num = 0;
            m_head->m_max_num = (mem_size_ - sizeof(BuffHead)) / sizeof(T);
        }
        m_buf = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(mem_) + sizeof(BuffHead));
        return true;
    }
};

template <size_t MAX_SIZE>
struct UnfixedRingBufData
{
protected:
    using IntType = typename FixIntType<MAX_SIZE>::IntType;

    IntType m_start = 0;
    IntType m_end = 0;
    IntType m_used_size = 0;
    IntType m_item_num = 0;
    /// todo 其实可以把max_size做一下对齐的，后面再加吧
    uint8_t m_buf[MAX_SIZE] = {0};

    IntType constexpr get_start() const { return m_start; }
    IntType constexpr get_end() const { return m_end; }
    IntType constexpr get_item_num() const { return m_item_num; }
    IntType constexpr get_used_size() const { return m_used_size; }
    IntType constexpr get_max_size() const { return MAX_SIZE; }

    inline void set_start(IntType start_) { m_start = start_; }
    inline void set_end(IntType end_) { m_end = end_; }

    inline void set_item_num(IntType item_num_) { m_item_num = item_num_; }
    inline void incr_item_num() { ++m_item_num; }
    inline void decr_item_num() { --m_item_num; }

    inline void set_used_size(IntType used_size_) { m_used_size = used_size_; }
    inline void incr_used_size(IntType incr_size_) { m_used_size += incr_size_; }
    inline void decr_used_size(IntType decr_size_) { m_used_size -= decr_size_; }
};

template <>
struct UnfixedRingBufData<0>
{
protected:
    using IntType = size_t;
    struct BuffHead
    {
        IntType m_start = 0;
        IntType m_end = 0;
        IntType m_used_size = 0;
        IntType m_item_num = 0;
        IntType m_max_size = 0;
    };
    BuffHead *m_head = nullptr;
    uint8_t *m_buf = nullptr;

    IntType constexpr get_start() const { return m_head->m_start; }
    IntType constexpr get_end() const { return m_head->m_end; }
    IntType constexpr get_item_num() const { return m_head->m_item_num; }
    IntType constexpr get_used_size() const { return m_head->m_used_size; }
    IntType constexpr get_max_size() const { return m_head->m_max_size; }

    inline void set_start(IntType start_) { m_head->m_start = start_; }
    inline void set_end(IntType end_) { m_head->m_end = end_; }

    inline void set_item_num(IntType item_num_) { m_head->m_item_num = item_num_; }
    inline void incr_item_num() { ++(m_head->m_item_num); }
    inline void decr_item_num() { --(m_head->m_item_num); }

    inline void set_used_size(IntType used_size_) { m_head->m_used_size = used_size_; }
    inline void incr_used_size(IntType incr_size_) { m_head->m_used_size += incr_size_; }
    inline void decr_used_size(IntType decr_size_) { m_head->m_used_size -= decr_size_; }

public:
    /// 调用者提供队列的内存
    bool init(void *mem_, size_t mem_size_, bool check_ = false)
    {
        if (!mem_ || mem_size_ < sizeof(BuffHead))
            return false;

        m_head = reinterpret_cast<BuffHead *>(mem_);
        if (check_)
        {
            if (m_head->m_max_size != (mem_size_ - sizeof(BuffHead)) || m_head->m_used_size > m_head->m_max_size)
                return false;
        }
        else
        {
            m_head->m_start = 0;
            m_head->m_end = 0;
            m_head->m_used_size = 0;
            m_head->m_item_num = 0;
            m_head->m_max_size = mem_size_ - sizeof(BuffHead);
        }

        m_buf = reinterpret_cast<uint8_t *>(mem_) + sizeof(BuffHead);
        return true;
    }
};

}  // namespace inner
}  // namespace pepper

#endif
