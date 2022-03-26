/*
 * * file name: mem_lru_set.h
 * * description: ...
 * * author: snow
 * * create time:2018  7 26
 * */

#ifndef _MEM_LRU_SET_H_
#define _MEM_LRU_SET_H_

//#include "inner/base_mem_lru_map.h"
#include "inner/base_specialization.h"
//#include "inner/policy.h"
#include "inner/lru_policy.h"
#include "inner/new_base_lru_map.h"

namespace pepper
{
template <typename T, size_t MAX_SIZE>
using MemLRUSet = exp::BaseMemLRUMap<inner::LRUPolicy<T, void, MAX_SIZE>>;

}  // namespace pepper

#endif
