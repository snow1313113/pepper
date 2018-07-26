/*
 * * file name: mem_lru_set.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  7 26
 * */

#ifndef MEM_LRU_SET_H
#define MEM_LRU_SET_H

#include "inner/base_mem_lru_set.h"

namespace Pepper
{

template<typename T, size_t MAX_SIZE, typename HASH = SimpleHash<T>, typename IS_EQUAL = IsEqual<T> >
struct MemLRUSet : public BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false>
{
    typedef BaseMemLRUSet<T, MAX_SIZE, HASH, IS_EQUAL, false> BaseType;
    typedef typename BaseType::IntType IntType;
    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Iterator Iterator;
};

}

#endif
