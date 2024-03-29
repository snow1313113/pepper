/*
 * * file name: mem_map_test.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 21
 * */

#ifndef _MEM_MAP_TEST_H_
#define _MEM_MAP_TEST_H_

#include "mem_map.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include "base_test_struct.h"
#include "gtest/gtest.h"

using namespace pepper;
using std::map;
using std::set;

TEST(MemMapTest, mem_map_test_normal)
{
    static const size_t MAX_SIZE = 1027;
    MemMap<uint32_t, TestNode, MAX_SIZE> mem_map;

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

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

        bool is_exist = mem_map.exist(i);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_map.insert(i, node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
        EXPECT_EQ(result_pair.first->second.b, node.b);

        is_exist = mem_map.exist(i);
        ASSERT_TRUE(is_exist);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(mem_map.size(), MAX_SIZE);
    ASSERT_TRUE(mem_map.full());
    ASSERT_FALSE(mem_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = mem_map.find(i);
        EXPECT_NE(iter, mem_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : mem_map)
    {
        EXPECT_EQ(it.first, it.second.a);
        EXPECT_EQ(it.second.a + it.second.b, it.second.c);
        EXPECT_EQ(it.second.d, 0);
        auto ret_pair = key_set.insert(it.first);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, MAX_SIZE);

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        EXPECT_NE(key_set.find(i), key_set.end());
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        key_set.erase(it);
        mem_map.erase(it);
    }

    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_map.begin(), end = mem_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_map.size(), count);

    // 测试清空
    mem_map.clear();

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), 0ul);
}

TEST(MemMapTest, mem_map_test_min_size)
{
    static const size_t MAX_SIZE = 20;
    MemMap<uint32_t, TestNode, MAX_SIZE> mem_map;

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

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

        bool is_exist = mem_map.exist(i);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_map.insert(i, node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
        EXPECT_EQ(result_pair.first->second.b, node.b);

        is_exist = mem_map.exist(i);
        ASSERT_TRUE(is_exist);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(mem_map.size(), MAX_SIZE);
    ASSERT_TRUE(mem_map.full());
    ASSERT_FALSE(mem_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = mem_map.find(i);
        EXPECT_NE(iter, mem_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : mem_map)
    {
        EXPECT_EQ(it.first, it.second.a);
        EXPECT_EQ(it.second.a + it.second.b, it.second.c);
        EXPECT_EQ(it.second.d, 0);
        auto ret_pair = key_set.insert(it.first);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, MAX_SIZE);

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        EXPECT_NE(key_set.find(i), key_set.end());
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        key_set.erase(it);
        mem_map.erase(it);
    }

    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_map.begin(), end = mem_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_map.size(), count);

    // 测试清空
    mem_map.clear();

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), 0ul);
}

static void zero_size_test(char* mem_, size_t mem_size_, size_t max_size_, size_t buckets_num_, set<size_t>& del_set_,
                           map<size_t, size_t>& node_rand_map_)
{
    MemMap<uint32_t, TestNode> mem_map;
    bool result = mem_map.init(mem_, mem_size_, max_size_, buckets_num_);
    ASSERT_TRUE(result);

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), max_size_);

    uint32_t seed = max_size_;
    for (size_t i = 1; i < max_size_ + 1; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        bool is_exist = mem_map.exist(i);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_map.insert(i, node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
        EXPECT_EQ(result_pair.first->second.b, node.b);

        is_exist = mem_map.exist(i);
        ASSERT_TRUE(is_exist);

        auto ret2_pair = node_rand_map_.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set_.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set_.empty())
        del_set_.insert(1);
}

TEST(MemMapTest, mem_map_test_0_size)
{
    static const size_t MAX_SIZE = 20;
    static const size_t BUCKETS_NUM = 17;

    size_t mem_size = MemMap<uint32_t, TestNode>::need_mem_size(MAX_SIZE, BUCKETS_NUM);
    ASSERT_TRUE(mem_size > 0);

    // 记录一批要del的node，后面做删除测试用
    set<size_t> del_set;
    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;

    std::unique_ptr<char[]> raw_mem(new char[mem_size]);
    // 在函数调用里面的MemMap使用，出了这个函数后里面的MemMap就会调用析构
    zero_size_test(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, del_set, node_rand_map);

    MemMap<uint32_t, TestNode> mem_map;
    bool result = mem_map.init(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, true);
    ASSERT_TRUE(result);

    EXPECT_EQ(mem_map.size(), MAX_SIZE);
    ASSERT_TRUE(mem_map.full());
    ASSERT_FALSE(mem_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = mem_map.find(i);
        EXPECT_NE(iter, mem_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : mem_map)
    {
        EXPECT_EQ(it.first, it.second.a);
        EXPECT_EQ(it.second.a + it.second.b, it.second.c);
        EXPECT_EQ(it.second.d, 0);
        auto ret_pair = key_set.insert(it.first);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, MAX_SIZE);

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        EXPECT_NE(key_set.find(i), key_set.end());
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        key_set.erase(it);
        mem_map.erase(it);
    }

    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_map.begin(), end = mem_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_map.size(), count);

    // 测试清空
    mem_map.clear();

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_map.size(), 0ul);
}

TEST(MemMapTest, mem_map_test_iterator)
{
    static const size_t MAX_SIZE = 1027;
    MemMap<uint32_t, size_t, MAX_SIZE> mem_map;

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_map.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
    }

    auto iter = std::find_if(mem_map.begin(), mem_map.end(), [=](const auto& tmp) { return tmp.first == 100; });
    EXPECT_NE(iter, mem_map.end());
}

TEST(MemMapTest, mem_map_test_iterator_min_size)
{
    static const size_t MAX_SIZE = 17;
    MemMap<uint32_t, size_t, MAX_SIZE> mem_map;

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_map.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
    }

    auto iter = std::find_if(mem_map.begin(), mem_map.end(), [=](const auto& tmp) { return tmp.first == 10; });
    EXPECT_NE(iter, mem_map.end());
}

TEST(MemMapTest, mem_map_test_insert)
{
    static const size_t MAX_SIZE = 101;
    MemMap<uint32_t, size_t, MAX_SIZE> mem_map;

    ASSERT_TRUE(mem_map.empty());
    ASSERT_FALSE(mem_map.full());
    EXPECT_EQ(mem_map.size(), 0ul);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_map.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
    }

    ASSERT_FALSE(mem_map.empty());
    ASSERT_TRUE(mem_map.full());
    EXPECT_EQ(mem_map.size(), MAX_SIZE);
    EXPECT_EQ(mem_map.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_map.insert(i + 1, (i + 1) * 100);
        ASSERT_FALSE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_map.end());
        auto& tmp = *(result_pair.first);
        EXPECT_EQ(tmp.first, i + 1);
        EXPECT_EQ(tmp.second, (i + 1) * 100);
    }
}

#endif
