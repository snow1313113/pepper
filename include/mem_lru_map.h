/*
 * * file name: mem_lru_map.h
 * * description: ...
 * * author: snow
 * * create time:2018  8 17
 * */

#ifndef _MEM_LRU_MAP_H_
#define _MEM_LRU_MAP_H_

//#include "inner/base_mem_lru_map.h"
#include "inner/base_specialization.h"
#include "inner/lru_policy.h"
#include "inner/new_base_lru_map.h"

namespace pepper
{
template <typename KEY, typename VALUE, size_t MAX_SIZE>
class MemLRUMap : public exp::BaseMemLRUMap<inner::LRUPolicy<KEY, VALUE, MAX_SIZE>>
{
public:
    using BaseType = exp::BaseMemLRUMap<inner::LRUPolicy<KEY, VALUE, MAX_SIZE>>;
    using Iterator = typename BaseType::Iterator;
    using DisuseCallback = typename BaseType::DisuseCallback;
    using BaseType::insert;

    /// 插入一个元素，如果存在则返回失败（其实我更喜欢直接返回bool）
    std::pair<Iterator, bool> insert(const KEY& key_, const VALUE& value_, bool force_ = false,
                                     const DisuseCallback& call_back_ = nullptr)
    {
        return BaseType::insert({key_, value_}, force_, call_back_);
    }
};

}  // namespace pepper

#endif
