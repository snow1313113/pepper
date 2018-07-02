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

template <int>
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
struct CalcBit
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

}

#endif
