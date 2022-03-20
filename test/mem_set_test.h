/*
 * * file name: mem_set_test.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 20
 * */

#ifndef _MEM_SET_TEST_H_
#define _MEM_SET_TEST_H_

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include "base_test_struct.h"
#include "gtest/gtest.h"
#include "mem_set.h"

using namespace pepper;
using std::map;
using std::set;

TEST(MemSetTest, mem_set_test_normal)
{
    static const size_t MAX_SIZE = 1027;
    MemSet<TestNode, MAX_SIZE> mem_set;

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

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

        bool is_exist = mem_set.exist(node);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        is_exist = mem_set.exist(node);
        ASSERT_TRUE(is_exist);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(mem_set.size(), MAX_SIZE);
    ASSERT_TRUE(mem_set.full());
    ASSERT_FALSE(mem_set.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        auto iter = mem_set.find(node);
        EXPECT_NE(iter, mem_set.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto&& it : mem_set)
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
        mem_set.erase(node);
    }

    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_set.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_set.begin(), end = mem_set.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(key_set.find(beg->a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_set.size(), count);

    // 测试清空
    mem_set.clear();

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), 0ul);
}

TEST(MemSetTest, mem_set_test_min_size)
{
    static const size_t MAX_SIZE = 20;
    MemSet<TestNode, MAX_SIZE> mem_set;

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

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

        bool is_exist = mem_set.exist(node);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        is_exist = mem_set.exist(node);
        ASSERT_TRUE(is_exist);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, node.b));
        ASSERT_TRUE(ret2_pair.second);

        if (node.b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(mem_set.size(), MAX_SIZE);
    ASSERT_TRUE(mem_set.full());
    ASSERT_FALSE(mem_set.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        auto iter = mem_set.find(node);
        EXPECT_NE(iter, mem_set.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto&& it : mem_set)
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
        mem_set.erase(node);
    }

    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_set.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_set.begin(), end = mem_set.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(key_set.find(beg->a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_set.size(), count);

    // 测试清空
    mem_set.clear();

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), 0ul);
}

namespace inner_mem_set_test
{
void zero_size_test(char* mem_, size_t mem_size_, size_t max_size_, size_t buckets_num_, set<size_t>& del_set_,
                    map<size_t, size_t>& node_rand_map_)
{
    MemSet<TestNode> mem_set;
    bool result = mem_set.init(mem_, mem_size_, max_size_, buckets_num_);
    ASSERT_TRUE(result);

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), max_size_);

    uint32_t seed = max_size_;
    for (size_t i = 1; i < max_size_ + 1; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        bool is_exist = mem_set.exist(node);
        ASSERT_FALSE(is_exist);

        auto result_pair = mem_set.insert(node);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
        EXPECT_EQ(result_pair.first->b, node.b);

        is_exist = mem_set.exist(node);
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

}  // namespace inner_mem_set_test

TEST(MemSetTest, mem_set_test_0_size)
{
    static const size_t MAX_SIZE = 20;
    static const size_t BUCKETS_NUM = 17;
    MemSet<TestNode> mem_set;

    size_t mem_size = MemSet<TestNode>::need_mem_size(MAX_SIZE, BUCKETS_NUM);
    ASSERT_TRUE(mem_size > 0);

    // 记录一批要del的node，后面做删除测试用
    set<size_t> del_set;
    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;

    std::unique_ptr<char[]> raw_mem(new char[mem_size]);

    inner_mem_set_test::zero_size_test(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, del_set, node_rand_map);

    bool result = mem_set.init(raw_mem.get(), mem_size, MAX_SIZE, BUCKETS_NUM, true);
    ASSERT_TRUE(result);

    EXPECT_EQ(mem_set.size(), MAX_SIZE);
    ASSERT_TRUE(mem_set.full());
    ASSERT_FALSE(mem_set.empty());

    for (size_t i = 1; i < MAX_SIZE + 1; ++i)
    {
        TestNode node;
        node.a = i;
        auto iter = mem_set.find(node);
        EXPECT_NE(iter, mem_set.end());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto&& it : mem_set)
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
        mem_set.erase(node);
    }

    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), MAX_SIZE - del_set.size());
    EXPECT_EQ(mem_set.size(), key_set.size());

    count = 0;
    // 测试迭代器的operator函数
    for (auto beg = mem_set.begin(), end = mem_set.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(key_set.find(beg->a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_set.size(), count);

    // 测试清空
    mem_set.clear();

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);
    EXPECT_EQ(mem_set.size(), 0ul);
}

/*
TEST(MemSetTest, mem_set_test_iterator)
{
    static const size_t MAX_SIZE = 1027;
    MemSet<uint32_t, size_t, MAX_SIZE> mem_set;

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_set.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
    }

    auto iter = std::find_if(mem_set.begin(), mem_set.end(), [=](const auto& tmp) { return tmp.first == 100; });
    EXPECT_NE(iter, mem_set.end());
}

TEST(MemSetTest, mem_set_test_iterator_min_size)
{
    static const size_t MAX_SIZE = 17;
    MemSet<uint32_t, size_t, MAX_SIZE> mem_set;

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_set.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
    }

    auto iter = std::find_if(mem_set.begin(), mem_set.end(), [=](const auto& tmp) { return tmp.first == 10; });
    EXPECT_NE(iter, mem_set.end());
}

TEST(MemSetTest, mem_set_test_insert)
{
    static const size_t MAX_SIZE = 101;
    MemSet<uint32_t, size_t, MAX_SIZE> mem_set;

    ASSERT_TRUE(mem_set.empty());
    ASSERT_FALSE(mem_set.full());
    EXPECT_EQ(mem_set.size(), 0ul);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_set.insert(i + 1, (i + 1) * 100);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
    }

    ASSERT_FALSE(mem_set.empty());
    ASSERT_TRUE(mem_set.full());
    EXPECT_EQ(mem_set.size(), MAX_SIZE);
    EXPECT_EQ(mem_set.capacity(), MAX_SIZE);

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto result_pair = mem_set.insert(i + 1, (i + 1) * 100);
        ASSERT_FALSE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_set.end());
        auto& tmp = *(result_pair.first);
        EXPECT_EQ(tmp.first, i + 1);
        EXPECT_EQ(tmp.second, (i + 1) * 100);
    }
}
*/

#endif
