/*
 * * file name: mem_lru_set_test.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 09
 * */

#ifndef _MEM_LRU_SET_TEST_H_
#define _MEM_LRU_SET_TEST_H_

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include "base_test_struct.h"
#include "gtest/gtest.h"
#include "mem_lru_set.h"

using namespace pepper;
using std::list;
using std::map;
using std::set;

TEST(MemLRUSetTest, mem_lru_set_test_normal)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(lru_set.size(), MAX_SIZE);
    ASSERT_TRUE(lru_set.full());
    ASSERT_FALSE(lru_set.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        auto iter = lru_set.find(node);
        EXPECT_NE(iter, lru_set.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto &it : lru_set)
    {
        EXPECT_EQ(it.a + it.b, it.c);
        EXPECT_EQ(it.d, 0);
        auto ret_pair = key_set.insert(it.a);
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
        TestNode node;
        node.a = it;
        lru_set.erase(node);
    }

    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_set.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(lru_set.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = lru_set.begin(), end = lru_set.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(key_set.find(beg->a), key_set.end());
        ++count;
    }
    EXPECT_EQ(lru_set.size(), count);

    // 测试清空
    lru_set.clear();

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_set.size(), 0ul);
}

TEST(MemLRUSetTest, mem_lru_set_test_normal_iterator)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUSet<BaseNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

    static const uint32_t INSERT_NUM = 10;
    map<uint32_t, uint32_t> node_map;
    uint32_t seed = MAX_SIZE;
    for (uint32_t i = 0; i < INSERT_NUM; ++i)
    {
        BaseNode node;
        node.key = i;
        node.base = rand_r(&seed);

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->base, node.base);

        auto ret2_pair = node_map.insert({i, node.base});
        ASSERT_TRUE(ret2_pair.second);
    }

    uint32_t index = 0;
    for (auto &&beg = lru_set.begin(), end = lru_set.end(); beg != end; ++beg)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = INSERT_NUM - index - 1;
        EXPECT_EQ(beg->key, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(beg->base, it->second);

        ++index;
    }

    index = 0;
    for (auto &&tmp : lru_set)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = INSERT_NUM - index - 1;
        EXPECT_EQ(tmp.key, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(tmp.base, it->second);

        ++index;
    }

    // 反过来激活一遍
    for (uint32_t i = 0; i < INSERT_NUM; ++i)
    {
        uint32_t key = INSERT_NUM - i - 1;
        BaseNode node;
        node.key = key;
        auto iter = lru_set.active(node);
        EXPECT_NE(iter, lru_set.end());
    }

    index = 0;
    for (auto &&beg = lru_set.begin(), end = lru_set.end(); beg != end; ++beg)
    {
        ASSERT_TRUE(index < INSERT_NUM);
        uint32_t key = index;
        EXPECT_EQ(beg->key, key);

        auto it = node_map.find(key);
        EXPECT_NE(it, node_map.end());
        EXPECT_EQ(beg->base, it->second);

        ++index;
    }
}

TEST(MemLRUSetTest, mem_lru_set_test_normal_disuse)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        active_list.push_back(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_set.disuse(1), 1ul);
        EXPECT_EQ(lru_set.size(), active_list.size());

        for (auto it : active_list)
        {
            TestNode node;
            node.a = it;
            auto iter = lru_set.find(node);
            EXPECT_NE(iter, lru_set.end());
        }
    };

    EXPECT_EQ(lru_set.size(), 0ul);
    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
}

TEST(MemLRUSetTest, mem_lru_set_test_normal_active)
{
    static const size_t MAX_SIZE = 1027;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        active_list.push_front(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    // 从最新插入的到最早插入的，全部active一遍，这样最早插入的反而最新被active
    for (auto it : active_list)
    {
        TestNode node;
        node.a = it;
        auto iter = lru_set.active(node);
        EXPECT_NE(iter, lru_set.end());
    }

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_set.disuse(1), 1ul);
        EXPECT_EQ(lru_set.size(), active_list.size());

        for (auto it : active_list)
        {
            TestNode node;
            node.a = it;
            auto iter = lru_set.find(node);
            EXPECT_NE(iter, lru_set.end());
        }
    };

    EXPECT_EQ(lru_set.size(), 0ul);
    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
}

TEST(MemLRUSetTest, mem_lru_set_test_min_size)
{
    static const size_t MAX_SIZE = 20;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(lru_set.size(), MAX_SIZE);
    ASSERT_TRUE(lru_set.full());
    ASSERT_FALSE(lru_set.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        auto iter = lru_set.find(node);
        EXPECT_NE(iter, lru_set.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto &it : lru_set)
    {
        EXPECT_EQ(it.a + it.b, it.c);
        EXPECT_EQ(it.d, 0);
        auto ret_pair = key_set.insert(it.a);
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
        TestNode node;
        node.a = it;
        lru_set.erase(node);
    }

    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_set.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(lru_set.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = lru_set.begin(), end = lru_set.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(key_set.find(beg->a), key_set.end());
        ++count;
    }
    EXPECT_EQ(lru_set.size(), count);

    // 测试清空
    lru_set.clear();

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);
    EXPECT_EQ(lru_set.size(), 0ul);
}

TEST(MemLRUSetTest, mem_lru_set_test_min_size_disuse)
{
    static const size_t MAX_SIZE = 20;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        active_list.push_back(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_set.disuse(1), 1ul);
        EXPECT_EQ(lru_set.size(), active_list.size());

        for (auto it : active_list)
        {
            TestNode node;
            node.a = it;
            auto iter = lru_set.find(node);
            EXPECT_NE(iter, lru_set.end());
        }
    };

    EXPECT_EQ(lru_set.size(), 0ul);
    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
}

TEST(MemLRUSetTest, mem_lru_set_test_min_size_active)
{
    static const size_t MAX_SIZE = 20;
    MemLRUSet<TestNode, MAX_SIZE> lru_set;

    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
    EXPECT_EQ(lru_set.size(), 0ul);
    EXPECT_EQ(lru_set.capacity(), MAX_SIZE);

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

        auto result_pair = lru_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, lru_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        active_list.push_front(i);
    }

    EXPECT_EQ(active_list.size(), MAX_SIZE);

    // 从最新插入的到最早插入的，全部active一遍，这样最早插入的反而最新被active
    for (auto it : active_list)
    {
        TestNode node;
        node.a = it;
        auto iter = lru_set.active(node);
        EXPECT_NE(iter, lru_set.end());
    }

    while (!active_list.empty())
    {
        active_list.pop_front();

        EXPECT_EQ(lru_set.disuse(1), 1ul);
        EXPECT_EQ(lru_set.size(), active_list.size());

        for (auto it : active_list)
        {
            TestNode node;
            node.a = it;
            auto iter = lru_set.find(node);
            EXPECT_NE(iter, lru_set.end());
        }
    };

    EXPECT_EQ(lru_set.size(), 0ul);
    ASSERT_TRUE(lru_set.empty());
    ASSERT_FALSE(lru_set.full());
}

#endif
