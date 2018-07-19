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
struct MemSet : public BaseMemSet<T, MAX_SIZE, HASH, (MAX_SIZE <= 100) >
{
    typedef typename BaseMemSet<T, MAX_SIZE, HASH, (MAX_SIZE <= 100) >::IntType IntType;
    typedef typename BaseMemSet<T, MAX_SIZE, HASH, (MAX_SIZE <= 100) >::ValueType ValueType;
    typedef typename BaseMemSet<T, MAX_SIZE, HASH, (MAX_SIZE <= 100) >::Iterator Iterator;
};

}

#endif
