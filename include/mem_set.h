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

template<typename T, size_t MAX_SIZE, typename HASH = SimpleHash<T> >
struct MemSet : public BaseMemSet<T, MAX_SIZE, HASH, (sizeof(T) > 4 && MAX_SIZE <= 40) || (sizeof(T) <= 4 && MAX_SIZE <= 50) >
{
    typedef BaseMemSet<T, MAX_SIZE, HASH, (sizeof(T) > 4 && MAX_SIZE <= 40) || (sizeof(T) <= 4 && MAX_SIZE <= 50) > BaseType;
    typedef typename BaseType:IntType IntType;
    typedef typename BaseType:ValueType ValueType;
    typedef typename BaseType:Iterator Iterator;
};

}

#endif
