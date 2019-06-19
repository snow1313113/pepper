/*
 * * file name: mem_lru_map_test.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 19
 * */

#ifndef _MEM_LRU_MAP_TEST_H_
#define _MEM_LRU_MAP_TEST_H_

#include <map>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "mem_lru_map.h"
#include "gtest/gtest.h"
#include "base_test_struct.h"

using namespace Pepper;
using std::map;
using std::set;

TEST(MemLRUMapTest, mem_lru_map_test)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    // 记录一批要del的node，后面做删除测试用
    set<size_t> del_set;
    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    uint32_t seed = MAX_SIZE;
    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        auto result_pair = lru_map.insert(i, node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_map.end());
        EXPECT_EQ(result_pair.first->second.b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }
 
}

#endif
