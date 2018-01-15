/*
 * * file name: traits_utils.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 15
 * */

#ifndef TRAITS_UTILS_H
#define TRAITS_UTILS_H

#include "head.h"

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
struct CalcByte
{
    static const size_t BYTE_NUM = CalcByte<(Size >> 8), (Size >> 8) != 0>::BYTE_NUM + 1;
};

template <size_t Size>
struct CalcByte<Size, false>
{
    static const size_t BYTE_NUM = 0;
};

template <size_t Size>
struct FixSize
{
    typedef typename SizeTraits< CalcByte<Size, (Size != 0)>::BYTE_NUM >::IntType IntType;
};


}

#endif
