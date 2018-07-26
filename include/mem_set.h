/*
 * * file name: mem_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018 7月 16
 * */

#ifndef MEM_SET_H
#define MEM_SET_H

#include "inner/base_mem_set.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE, typename HASH = SimpleHash<T>, typename IS_EQUAL = IsEqual<T> >
struct MemSet : public BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, IsMinSizeMemSet<T, MAX_SIZE>::IS_MIN >
{
    typedef BaseMemSet<T, MAX_SIZE, HASH, IS_EQUAL, IsMinSizeMemSet<T, MAX_SIZE>::IS_MIN > BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;
};

}

#endif
