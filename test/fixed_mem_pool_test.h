/*
 * * file name: fixed_mem_pool_test.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 01
 * */

#ifndef _FIXED_MEM_POOL_TEST_H_
#define _FIXED_MEM_POOL_TEST_H_

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include "base_test_struct.h"
#include "fixed_mem_pool.h"
#include "gtest/gtest.h"

using namespace Pepper;
using std::map;
using std::set;

// 定义时知道node大小的测试
TEST(FixedMemPoolTest, know_node_size)
{
    size_t max_num = 1451;
    size_t mem_size = FixedMemPool<TestNode>::calc_need_size(max_num);
    uint8_t* mem = new uint8_t[mem_size];
    FixedMemPool<TestNode> mem_pool;

    bool result = mem_pool.init(mem, mem_size, max_num);
    ASSERT_TRUE(result);
    ASSERT_TRUE(mem_pool.empty());
    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), 0);
    EXPECT_EQ(mem_pool.node_size(), sizeof(TestNode));

    // 记录下每个node的ref，后面测试int_2_ptr转回来的结果是否正确
    map<size_t, uint32_t> index_map;
    // 记录要del的node，后面做删除测试用
    set<TestNode*> del_set;
    uint32_t seed = max_num;
    for (size_t i = 1; i < max_num + 1; ++i)
    {
        auto p = mem_pool.alloc();
        EXPECT_NE(p, nullptr);
        p->a = i;
        p->b = rand_r(&seed);
        p->c = p->a + p->b;
        p->d = 0;
        seed = p->b;

        size_t index = mem_pool.ptr_2_int(p);
        EXPECT_NE(index, 0);

        auto ret_pair = index_map.insert(std::make_pair(index, p->c));
        ASSERT_TRUE(ret_pair.second);

        if (p->b % 3 == 0)
            del_set.insert(p);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(&(*(mem_pool.begin())));

    EXPECT_EQ(mem_pool.size(), max_num);
    ASSERT_TRUE(mem_pool.full());
    EXPECT_EQ(mem_pool.alloc(), nullptr);
    EXPECT_EQ(mem_pool.size(), max_num);

    // 记录下每个node的a字段，后面测试是否node都没错
    set<uint32_t> a_set;
    size_t count = 0;
    for (auto beg = mem_pool.begin(), end = mem_pool.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        auto ret_pair = a_set.insert(beg->a);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, max_num);

    for (size_t i = 1; i < max_num + 1; ++i)
    {
        EXPECT_NE(a_set.find(i), a_set.end());
    }

    for (auto it : index_map)
    {
        size_t index = it.first;
        auto p = mem_pool.int_2_ptr(index);
        EXPECT_NE(p, nullptr);
        EXPECT_EQ(p->c, it.second);
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        a_set.erase(it->a);
        ASSERT_TRUE(mem_pool.free(it));
    }

    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.size(), a_set.size());

    count = 0;
    for (auto& node : mem_pool)
    {
        EXPECT_EQ(node.a + node.b, node.c);
        EXPECT_EQ(node.d, 0);
        EXPECT_NE(a_set.find(node.a), a_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.size(), count);

    // 测试init的check_参数
    result = mem_pool.init(mem, mem_size, max_num, true);
    ASSERT_TRUE(result);

    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.size(), a_set.size());
    EXPECT_EQ(mem_pool.node_size(), sizeof(TestNode));

    // 重新看看每个node对不对
    count = 0;
    for (auto beg = mem_pool.begin(), end = mem_pool.end(); beg != end; ++beg)
    {
        EXPECT_EQ((*beg).a + beg->b, beg->c);
        EXPECT_EQ((*beg).d, 0);
        EXPECT_NE(a_set.find(beg->a), a_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.size(), count);

    // 测试清空
    mem_pool.clear();

    ASSERT_TRUE(mem_pool.empty());
    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), 0);
    EXPECT_EQ(mem_pool.node_size(), sizeof(TestNode));
}

// init时指定node大小的测试
TEST(FixedMemPoolTest, unknow_node_size)
{
    size_t max_num = 1451;
    size_t mem_size = FixedMemPool<BaseNode>::calc_need_size(max_num, sizeof(TestNode));
    uint8_t* mem = new uint8_t[mem_size];
    FixedMemPool<BaseNode> mem_pool;

    bool result = mem_pool.init(mem, mem_size, max_num, sizeof(TestNode));
    ASSERT_TRUE(result);
    ASSERT_TRUE(mem_pool.empty());
    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), 0);
    EXPECT_EQ(mem_pool.node_size(), sizeof(TestNode));

    // 记录下每个node的ref，后面测试int_2_ptr转回来的结果是否正确
    map<size_t, uint32_t> index_map;
    // 记录要del的node，后面做删除测试用
    set<BaseNode*> del_set;
    uint32_t seed = max_num;
    for (size_t i = 1; i < max_num + 1; ++i)
    {
        auto p = static_cast<TestNode*>(mem_pool.alloc());
        EXPECT_NE(p, nullptr);
        p->a = i;
        p->b = rand_r(&seed);
        p->c = p->a + p->b;
        p->d = 0;
        seed = p->b;

        size_t index = mem_pool.ptr_2_int(p);
        EXPECT_NE(index, 0);

        auto ret_pair = index_map.insert(std::make_pair(index, p->c));
        ASSERT_TRUE(ret_pair.second);

        if (p->b % 3 == 0)
            del_set.insert(p);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(&(*(mem_pool.begin())));

    EXPECT_EQ(mem_pool.size(), max_num);
    ASSERT_TRUE(mem_pool.full());
    EXPECT_EQ(mem_pool.alloc(), nullptr);
    EXPECT_EQ(mem_pool.size(), max_num);

    // 记录下每个node的a字段，后面测试是否node都没错
    set<uint32_t> a_set;
    size_t count = 0;
    for (auto beg = mem_pool.begin(), end = mem_pool.end(); beg != end; ++beg)
    {
        const TestNode* p = static_cast<const TestNode*>(&(*beg));
        EXPECT_EQ(p->a + p->b, p->c);
        EXPECT_EQ(p->d, 0);
        auto ret_pair = a_set.insert(p->a);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, max_num);

    for (size_t i = 1; i < max_num + 1; ++i)
    {
        EXPECT_NE(a_set.find(i), a_set.end());
    }

    for (auto it : index_map)
    {
        size_t index = it.first;
        auto p = mem_pool.int_2_ptr(index);
        EXPECT_NE(p, nullptr);
        EXPECT_EQ(static_cast<const TestNode*>(p)->c, it.second);
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        const TestNode* p = static_cast<const TestNode*>(it);
        a_set.erase(p->a);
        ASSERT_TRUE(mem_pool.free(it));
    }

    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.size(), a_set.size());

    count = 0;
    for (auto beg = mem_pool.begin(), end = mem_pool.end(); beg != end; ++beg)
    {
        const TestNode* p = static_cast<const TestNode*>(&(*beg));
        EXPECT_EQ(p->a + p->b, p->c);
        EXPECT_EQ(p->d, 0);
        EXPECT_NE(a_set.find(p->a), a_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.size(), count);

    // 测试init的check_参数
    result = mem_pool.init(mem, mem_size, max_num, sizeof(TestNode), true);
    ASSERT_TRUE(result);

    ASSERT_FALSE(mem_pool.full());
    EXPECT_EQ(mem_pool.capacity(), max_num);
    EXPECT_EQ(mem_pool.size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.size(), a_set.size());
    EXPECT_EQ(mem_pool.node_size(), sizeof(TestNode));

    // 重新看看每个node对不对
    count = 0;
    for (auto beg = mem_pool.begin(), end = mem_pool.end(); beg != end; ++beg)
    {
        const TestNode* p = static_cast<const TestNode*>(&(*beg));
        EXPECT_EQ(p->a + p->b, p->c);
        EXPECT_EQ(p->d, 0);
        EXPECT_NE(a_set.find(p->a), a_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.size(), count);
}

#endif
