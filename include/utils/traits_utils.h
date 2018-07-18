/*
 * * file name: traits_utils.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 15
 * */

#ifndef TRAITS_UTILS_H
#define TRAITS_UTILS_H

#include "../head.h"

namespace Pepper
{

template <size_t>
struct SizeTraits
{
    typedef size_t IntType;
};

template <>
struct SizeTraits<0>
{
    typedef uint8_t IntType;
};

template <>
struct SizeTraits<1>
{
    typedef uint8_t IntType;
};

template <>
struct SizeTraits<2>
{
    typedef uint16_t IntType;
};

template <>
struct SizeTraits<3>
{
    typedef uint32_t IntType;
};

template <>
struct SizeTraits<4>
{
    typedef uint32_t IntType;
};

template <size_t Size, bool IsZero>
struct CalcBit;

template <size_t Size>
struct CalcBit<Size, true>
{
    static const size_t BIT_NUM = CalcBit<(Size >> 1), (Size >> 1) != 0>::BIT_NUM + 1;
};

template <size_t Size>
struct CalcBit<Size, false>
{
    static const size_t BIT_NUM = 0;
};

template <size_t Power, size_t N>
struct PowerOfN
{
    static const size_t RESULT = PowerOfN<Power, N-1>::RESULT * Power;
};

template <size_t Power>
struct PowerOfN<Power, 0>
{
    static const size_t RESULT = 1;
};

// 根据要表示的数量选择一个合适字节的INT类型
template <size_t Size>
struct FixIntType
{
    typedef typename SizeTraits< (CalcBit<Size, (Size != 0)>::BIT_NUM + 7) / 8 >::IntType IntType;
};

//////////////////////////////////////////////////////////////////////////////////////

// 类成员偏移，只针对trivial的类有效
template<typename MEMBER_T, typename CLASS_T>
size_t offset_of(MEMBER_T CLASS_T::*member)
{
    static CLASS_T object;
    return reinterpret_cast<size_t>(&(object.*member)) - reinterpret_cast<size_t>(&object);
}

// 根据成员指针，返回类实例地址，只针对trivial的类有效
template<typename MEMBER_T, typename CLASS_T>
inline CLASS_T * contaner_of(const MEMBER_T * ptr, MEMBER_T CLASS_T::*member)
{
    return reinterpret_cast<CLASS_T *>(reinterpret_cast<size_t>(ptr) - offset_of(member));
}

//////////////////////////////////////////////////////////////////////////////
template<size_t NUM, size_t MOD, bool IS_PRIME>
struct RealIsPrime;

// todo 可以引入6素数算法，就是只是判断是否是 6x+1 和 6x+5的倍数
template<size_t NUM, size_t MOD>
struct RealIsPrime<NUM, MOD, true>
{
    enum {RESULT = RealIsPrime<NUM, MOD - 1, (NUM % MOD != 0)>::RESULT};
};

template<size_t NUM, size_t MOD>
struct RealIsPrime<NUM, MOD, false>
{
    enum {RESULT = false};
};

template<size_t NUM>
struct RealIsPrime<NUM, 2, true>
{
    enum {RESULT = true};
};

template<size_t NUM>
struct RealIsPrime<NUM, 1, true>
{
    enum {RESULT = true};
};

template<size_t NUM>
struct IsPrime
{
    enum {RESULT = RealIsPrime<NUM, (NUM + 1) / 2, (NUM % 2 != 0) && (NUM % 3 != 0) && true>::RESULT};
};

template<>
struct IsPrime<2>
{
    enum {RESULT = true};
};

template<>
struct IsPrime<3>
{
    enum {RESULT = true};
};

template<size_t NUM, bool IS_PRIME>
struct NearByPrime;

template<size_t NUM>
struct NearByPrime<NUM, true>
{
    static const size_t PRIME = NUM;
};

template<size_t NUM>
struct NearByPrime<NUM, false>
{
    static const size_t PRIME = NearByPrime<NUM - 1, IsPrime<NUM - 1>::RESULT>::PRIME;
};

template<size_t NUM>
struct CalcPrime
{
    static const size_t PRIME = NearByPrime<NUM, IsPrime<NUM>::RESULT>::PRIME;
};

template<>
struct CalcPrime<1>
{
    static const size_t PRIME = 1;
};


//////////////////////////////////////////////////////////////////////////////

template <class keytpe>
struct SimpleHash;

template<>
struct SimpleHash<int8_t>
{
    size_t operator()(int8_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<uint8_t>
{
    size_t operator()(uint8_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<int16_t>
{
    size_t operator()(int16_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<uint16_t>
{
    size_t operator()(uint16_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<int32_t>
{
    size_t operator()(int32_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<uint32_t>
{
    size_t operator()(uint32_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<int64_t>
{
    size_t operator()(int64_t x) const
    {
        return static_cast<size_t>(x);
    }
};

template<>
struct SimpleHash<uint64_t>
{
    size_t operator()(uint64_t x) const
    {
        return static_cast<size_t>(x);
    }
};

//////////////////////////////////////////////////////////////////////////////

/// 获取Key
template <class T>
struct ExtractKey
{
    typedef T KeyType;
    const KeyType & operator()(const T & x) const
    {
        return x;
    }
};


}

#endif
