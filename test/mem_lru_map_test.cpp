/*
 * * file name: mem_lru_map_test.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 19
 * */

#ifndef _MEM_LRU_MAP_TEST_H_
#define _MEM_LRU_MAP_TEST_H_

#include "mem_lru_map.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include "base_test_struct.h"
#include "gtest/gtest.h"

using namespace pepper;
using std::list;
using std::map;
using std::set;

TEST(MemLRUMapTest, mem_lru_map_test_normal)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

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

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(lru_map.size(), MAX_SIZE);
    ASSERT_TRUE(lru_map.full());
    ASSERT_FALSE(lru_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = lru_map.find(i);
        EXPECT_NE(iter, lru_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : lru_map)
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
        lru_map.erase(it);
    }

    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(lru_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = lru_map.begin(), end = lru_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(lru_map.size(), count);

    // 测试清空
    lru_map.clear();

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), 0ul);
}

TEST(MemLRUMapTest, mem_lru_map_test_normal_iterator)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUMap<uint32_t, BaseNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

    static const uint32_t INSERT_NUM = 10;
    map<uint32_t, uint32_t> node_map;
    uint32_t seed = MAX_SIZE;
    for (uint32_t i = 0; i < INSERT_NUM; ++i)
    {
        BaseNode node;
        node.base = rand_r(&seed);

        auto result_pair = lru_map.insert(i, node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_map.end());
        EXPECT_EQ(result_pair.first->second.base, node.base);

        auto ret2_pair = node_map.insert({i, node.base});
        ASSERT_TRUE(ret2_pair.second);
    }

    uint32_t index = 0;
    for (auto &&beg = lru_map.begin(), end = lru_map.end(); beg != end; ++beg)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = INSERT_NUM - index - 1;
        EXPECT_EQ(beg->first, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(beg->second.base, it->second);

        ++index;
    }

    index = 0;
    for (auto&& tmp : lru_map)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = INSERT_NUM - index - 1;
        EXPECT_EQ(tmp.first, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(tmp.second.base, it->second);

        ++index;
    }

    // 反过来激活一遍
    for (uint32_t i = 0; i < INSERT_NUM; ++i)
    {
        uint32_t key = INSERT_NUM - i - 1;
        auto iter = lru_map.active(key);
        EXPECT_NE(iter, lru_map.end());
    }

    index = 0;
    for (auto &&beg = lru_map.begin(), end = lru_map.end(); beg != end; ++beg)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = index;
        EXPECT_EQ(beg->first, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(beg->second.base, it->second);

        ++index;
    }
}

TEST(MemLRUMapTest, mem_lru_map_test_normal_disuse)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    // 淘汰队列
    list<size_t> active_list;
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

        active_list.push_back(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_map.disuse(1), 1ul);
        EXPECT_EQ(lru_map.size(), active_list.size());

        for (auto it : active_list)
        {
            auto iter = lru_map.find(it);
            EXPECT_NE(iter, lru_map.end());
        }
    };

    EXPECT_EQ(lru_map.size(), 0ul);
    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
}

TEST(MemLRUMapTest, mem_lru_map_test_normal_active)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    // 淘汰队列
    list<size_t> active_list;
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

        active_list.push_front(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    // 从最新插入的到最早插入的，全部active一遍，这样最早插入的反而最新被active
    for (auto it : active_list)
    {
        auto iter = lru_map.active(it);
        EXPECT_NE(iter, lru_map.end());
    }

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_map.disuse(1), 1ul);
        EXPECT_EQ(lru_map.size(), active_list.size());

        for (auto it : active_list)
        {
            auto iter = lru_map.find(it);
            EXPECT_NE(iter, lru_map.end());
        }
    };

    EXPECT_EQ(lru_map.size(), 0ul);
    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
}

TEST(MemLRUMapTest, mem_lru_map_test_min_size)
{
    static const size_t MAX_SIZE = 20;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

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

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(lru_map.size(), MAX_SIZE);
    ASSERT_TRUE(lru_map.full());
    ASSERT_FALSE(lru_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = lru_map.find(i);
        EXPECT_NE(iter, lru_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : lru_map)
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
        lru_map.erase(it);
    }

    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(lru_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = lru_map.begin(), end = lru_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(lru_map.size(), count);

    // 测试清空
    lru_map.clear();

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), 0ul);
}

TEST(MemLRUMapTest, mem_lru_map_test_min_size_disuse)
{
    static const size_t MAX_SIZE = 20;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    // 淘汰队列
    list<size_t> active_list;
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

        active_list.push_back(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_map.disuse(1), 1ul);
        EXPECT_EQ(lru_map.size(), active_list.size());

        for (auto it : active_list)
        {
            auto iter = lru_map.find(it);
            EXPECT_NE(iter, lru_map.end());
        }
    };

    EXPECT_EQ(lru_map.size(), 0ul);
    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
}

TEST(MemLRUMapTest, mem_lru_map_test_min_size_active)
{
    static const size_t MAX_SIZE = 20;
    MemLRUMap<uint32_t, TestNode, MAX_SIZE> lru_map;

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);

    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    // 淘汰队列
    list<size_t> active_list;
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

        active_list.push_front(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    // 从最新插入的到最早插入的，全部active一遍，这样最早插入的反而最新被active
    for (auto it : active_list)
    {
        auto iter = lru_map.active(it);
        EXPECT_NE(iter, lru_map.end());
    }

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_map.disuse(1), 1ul);
        EXPECT_EQ(lru_map.size(), active_list.size());

        for (auto it : active_list)
        {
            auto iter = lru_map.find(it);
            EXPECT_NE(iter, lru_map.end());
        }
    };

    EXPECT_EQ(lru_map.size(), 0ul);
    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
}

static void zero_size_test(char* mem_, size_t mem_size_, size_t max_size_, size_t buckets_num_, set<size_t>& del_set_,
                           map<size_t, size_t>& node_rand_map_)
{
    MemLRUMap<uint32_t, TestNode> lru_map;
    bool result = lru_map.init(mem_, mem_size_, max_size_, buckets_num_);
    ASSERT_TRUE(result);

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.size(), 0ul);
    EXPECT_EQ(lru_map.capacity(), max_size_);

    uint32_t seed = max_size_;
    for (size_t i = 1; i < max_size_ + 1; ++i)
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

        auto ret2_pair = node_rand_map_.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set_.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set_.empty())
        del_set_.insert(1);
}

TEST(MemLRUMapTest, mem_lru_map_test_0_size)
{
    static const size_t MAX_SIZE = 20;
    static const size_t BUCKETS_NUM = 17;

    size_t mem_size = MemLRUMap<uint32_t, TestNode>::need_mem_size(MAX_SIZE, BUCKETS_NUM);
    ASSERT_TRUE(mem_size > 0);

    // 记录一批要del的node，后面做删除测试用
    set<size_t> del_set;
    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;

    std::unique_ptr<char[]> raw_mem(new char[mem_size]);
    // 在函数调用里面的LRUMap使用，出了这个函数后里面的LRUMap就会调用析构
    zero_size_test(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, del_set, node_rand_map);

    // 新的LRUMap对象重新init
    MemLRUMap<uint32_t, TestNode> lru_map;
    bool result = lru_map.init(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, true);
    ASSERT_TRUE(result);

    EXPECT_EQ(lru_map.size(), MAX_SIZE);
    ASSERT_TRUE(lru_map.full());
    ASSERT_FALSE(lru_map.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        auto iter = lru_map.find(i);
        EXPECT_NE(iter, lru_map.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto& it : lru_map)
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
        lru_map.erase(it);
    }

    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(lru_map.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = lru_map.begin(), end = lru_map.end(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(lru_map.size(), count);

    // 测试清空
    lru_map.clear();

    ASSERT_TRUE(lru_map.empty());
    ASSERT_FALSE(lru_map.full());
    EXPECT_EQ(lru_map.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_map.size(), 0ul);
}

#endif
