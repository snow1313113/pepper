/*
 * * file name: mem_lru_map.h
 * * description: ...
 * * author: snow
 * * create time:2018  8 17
 * */

#ifndef _MEM_LRU_MAP_H_
#define _MEM_LRU_MAP_H_

#include "inner/base_mem_lru_map.h"
#include "inner/base_specialization.h"
#include "inner/policy.h"

namespace pepper
{
template <typename KEY, typename VALUE, size_t MAX_SIZE, typename POLICY = DefaultPolicy<KEY>>
using MemLRUMap = inner::BaseMemLRUMap<KEY, VALUE, MAX_SIZE, POLICY>;

}  // namespace pepper

#endif
