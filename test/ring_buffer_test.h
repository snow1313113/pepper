/*
 * * file name: ring_buffer_test.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 01
 * */

#ifndef _RING_BUFFER_TEST_H_
#define _RING_BUFFER_TEST_H_

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "base_test_struct.h"
#include "fixed_ring_buf.h"
#include "gtest/gtest.h"
#include "unfixed_ring_buf.h"

using namespace pepper;
using std::map;
using std::set;
using std::vector;

TEST(RingBufferTest, fixed_ring_buffer_1024)
{
    static const size_t MAX_SIZE = 1024;
    FixedRingBuf<TestNode, MAX_SIZE> ring_buf;

    ASSERT_TRUE(ring_buf.empty());
    ASSERT_FALSE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), 0);
    EXPECT_EQ(ring_buf.capacity(), MAX_SIZE);

    uint32_t seed = MAX_SIZE;
    vector<uint32_t> index_2_node;
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        ASSERT_TRUE(ring_buf.push(node));
        index_2_node.push_back(node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    TestNode node;
    node.a = MAX_SIZE + 1;
    ASSERT_FALSE(ring_buf.push(node));

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.front(i - 1);
        EXPECT_EQ(index_2_node[i - 1], node.c);
    }

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.back(i - 1);
        EXPECT_EQ(index_2_node[MAX_SIZE - i], node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    for (size_t i = 0; i <= MAX_SIZE / 2; ++i)
        ring_buf.pop();

    EXPECT_EQ(ring_buf.size(), MAX_SIZE - (MAX_SIZE / 2 + 1));

    ring_buf.clear();
    ASSERT_TRUE(ring_buf.empty());
    EXPECT_EQ(ring_buf.size(), 0);

    // 测试循环push
    index_2_node.clear();
    for (size_t i = 1; i <= 2 * MAX_SIZE; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        ASSERT_TRUE(ring_buf.push(node, true));
        index_2_node.push_back(node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    size_t begin_index = index_2_node.size() - ring_buf.size();
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.front(i - 1);
        EXPECT_EQ(index_2_node[begin_index + i - 1], node.c);
    }

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.back(i - 1);
        EXPECT_EQ(index_2_node[index_2_node.size() - i], node.c);
    }
}

// 特化版本的测试
TEST(RingBufferTest, fixed_ring_buffer_0)
{
    static const size_t MAX_SIZE = 1024;
    FixedRingBuf<TestNode> ring_buf;

    size_t mem_size = FixedRingBuf<TestNode>::mem_size(MAX_SIZE);
    auto p = new uint8_t[mem_size];
    ASSERT_TRUE(ring_buf.init(p, mem_size));

    ASSERT_TRUE(ring_buf.empty());
    ASSERT_FALSE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), 0);
    EXPECT_EQ(ring_buf.capacity(), MAX_SIZE);

    uint32_t seed = MAX_SIZE;
    vector<uint32_t> index_2_node;
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        ASSERT_TRUE(ring_buf.push(node));
        index_2_node.push_back(node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    TestNode node;
    node.a = MAX_SIZE + 1;
    ASSERT_FALSE(ring_buf.push(node));

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.front(i - 1);
        EXPECT_EQ(index_2_node[i - 1], node.c);
    }

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = ring_buf.back(i - 1);
        EXPECT_EQ(index_2_node[MAX_SIZE - i], node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    for (size_t i = 0; i <= MAX_SIZE / 2; ++i)
        ring_buf.pop();

    EXPECT_EQ(ring_buf.size(), MAX_SIZE - (MAX_SIZE / 2 + 1));

    ring_buf.clear();
    ASSERT_TRUE(ring_buf.empty());
    EXPECT_EQ(ring_buf.size(), 0);

    // 测试循环push
    index_2_node.clear();
    for (size_t i = 1; i <= 2 * MAX_SIZE; ++i)
    {
        TestNode node;
        node.a = i;
        node.b = rand_r(&seed);
        node.c = node.a + node.b;
        node.d = 0;
        seed = node.b;

        ASSERT_TRUE(ring_buf.push(node, true));
        index_2_node.push_back(node.c);
    }

    ASSERT_TRUE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), MAX_SIZE);

    // 新的RingBuf对象，测试重新初始化的check参数
    FixedRingBuf<TestNode> reinit_ring_buf;
    size_t new_mem_size = FixedRingBuf<TestNode>::mem_size(MAX_SIZE);
    ASSERT_TRUE(reinit_ring_buf.init(p, new_mem_size, true));

    ASSERT_TRUE(reinit_ring_buf.full());
    EXPECT_EQ(reinit_ring_buf.size(), MAX_SIZE);

    size_t begin_index = index_2_node.size() - reinit_ring_buf.size();
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = reinit_ring_buf.front(i - 1);
        EXPECT_EQ(index_2_node[begin_index + i - 1], node.c);
    }

    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        auto& node = reinit_ring_buf.back(i - 1);
        EXPECT_EQ(index_2_node[index_2_node.size() - i], node.c);
    }
}

// 测试不定长元素队列
TEST(RingBufferTest, un_fixed_ring_buffer_1024)
{
    static const size_t MAX_SIZE = 1024;
    UnfixedRingBuf<MAX_SIZE> ring_buf;

    ASSERT_TRUE(ring_buf.empty());
    ASSERT_FALSE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), 0);
    EXPECT_EQ(ring_buf.capacity(), MAX_SIZE);
    EXPECT_EQ(ring_buf.get_num(), 0);

    uint32_t seed = MAX_SIZE;
    vector<uint32_t> index_2_node;
    size_t max_num = 0;
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        if (i % 3 == 0)
        {
            TestNode node;
            node.base = i;
            node.a = i;
            node.b = rand_r(&seed);
            node.c = node.a + node.b;
            node.d = 0;
            seed = node.b;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node)) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(node.c);
        }
        else
        {
            BaseNode node;
            node.base = i;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node)) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(0);
        }
    }
    EXPECT_EQ(ring_buf.get_num(), max_num);
    ASSERT_FALSE(ring_buf.empty());

    // 测试front
    for (size_t i = 1; i <= max_num; ++i)
    {
        size_t len = 0;
        auto data = ring_buf.front(len, i - 1);
        if (i % 3 == 0)
        {
            EXPECT_EQ(len, sizeof(TestNode));
            const TestNode* node = reinterpret_cast<const TestNode*>(data);
            EXPECT_EQ(node->base, node->a);
            EXPECT_EQ(node->base, i);
            EXPECT_EQ(index_2_node[i - 1], node->c);
        }
        else
        {
            EXPECT_EQ(len, sizeof(BaseNode));
            const BaseNode* node = reinterpret_cast<const BaseNode*>(data);
            EXPECT_EQ(node->base, i);
        }
    }

    size_t len = 0;
    EXPECT_EQ(ring_buf.front(len, max_num), nullptr);

    for (size_t i = 0; i < (max_num + 1) / 2; ++i)
        ring_buf.pop();

    EXPECT_EQ(ring_buf.get_num(), max_num - (max_num + 1) / 2);

    ring_buf.clear();
    ASSERT_TRUE(ring_buf.empty());
    EXPECT_EQ(ring_buf.size(), 0);
    EXPECT_EQ(ring_buf.get_num(), 0);

    // 测试循环push
    index_2_node.clear();
    for (size_t i = 1; i <= 2 * MAX_SIZE; ++i)
    {
        if (i % 3 == 0)
        {
            TestNode node;
            node.base = i;
            node.a = i;
            node.b = rand_r(&seed);
            node.c = node.a + node.b;
            node.d = 0;
            seed = node.b;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node), true) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(node.c);
        }
        else
        {
            BaseNode node;
            node.base = i;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node), true) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(0);
        }
    }

    size_t num = ring_buf.get_num();
    size_t begin_index = index_2_node.size() - num;
    for (size_t i = 1; i <= num; ++i)
    {
        size_t len = 0;
        auto data = ring_buf.front(len, i - 1);
        if ((i + begin_index) % 3 == 0)
        {
            EXPECT_EQ(len, sizeof(TestNode));
            const TestNode* node = reinterpret_cast<const TestNode*>(data);
            EXPECT_EQ(node->base, node->a);
            EXPECT_EQ(node->base, i + begin_index);
            EXPECT_EQ(index_2_node[begin_index + i - 1], node->c);
        }
        else
        {
            EXPECT_EQ(len, sizeof(BaseNode));
            const BaseNode* node = reinterpret_cast<const BaseNode*>(data);
            EXPECT_EQ(node->base, i + begin_index);
        }
    }
}

// 测试不定长元素队列的特化版本
TEST(RingBufferTest, un_fixed_ring_buffer_0)
{
    static const size_t MAX_SIZE = 1024;
    UnfixedRingBuf<> ring_buf;

    auto p = new uint8_t[MAX_SIZE];
    ASSERT_TRUE(ring_buf.init(p, MAX_SIZE));

    ASSERT_TRUE(ring_buf.empty());
    ASSERT_FALSE(ring_buf.full());
    EXPECT_EQ(ring_buf.size(), 0);
    ASSERT_TRUE(ring_buf.capacity() <= MAX_SIZE);
    EXPECT_EQ(ring_buf.get_num(), 0);

    uint32_t seed = MAX_SIZE;
    vector<uint32_t> index_2_node;
    size_t max_num = 0;
    for (size_t i = 1; i <= MAX_SIZE; ++i)
    {
        if (i % 3 == 0)
        {
            TestNode node;
            node.base = i;
            node.a = i;
            node.b = rand_r(&seed);
            node.c = node.a + node.b;
            node.d = 0;
            seed = node.b;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node)) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(node.c);
        }
        else
        {
            BaseNode node;
            node.base = i;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node)) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(0);
        }
    }
    EXPECT_EQ(ring_buf.get_num(), max_num);
    ASSERT_FALSE(ring_buf.empty());

    // 测试front
    for (size_t i = 1; i <= max_num; ++i)
    {
        size_t len = 0;
        auto data = ring_buf.front(len, i - 1);
        if (i % 3 == 0)
        {
            EXPECT_EQ(len, sizeof(TestNode));
            const TestNode* node = reinterpret_cast<const TestNode*>(data);
            EXPECT_EQ(node->base, node->a);
            EXPECT_EQ(node->base, i);
            EXPECT_EQ(index_2_node[i - 1], node->c);
        }
        else
        {
            EXPECT_EQ(len, sizeof(BaseNode));
            const BaseNode* node = reinterpret_cast<const BaseNode*>(data);
            EXPECT_EQ(node->base, i);
        }
    }

    size_t len = 0;
    EXPECT_EQ(ring_buf.front(len, max_num), nullptr);

    for (size_t i = 0; i < (max_num + 1) / 2; ++i)
        ring_buf.pop();

    EXPECT_EQ(ring_buf.get_num(), max_num - (max_num + 1) / 2);

    ring_buf.clear();
    ASSERT_TRUE(ring_buf.empty());
    EXPECT_EQ(ring_buf.size(), 0);
    EXPECT_EQ(ring_buf.get_num(), 0);

    // 测试循环push
    index_2_node.clear();
    for (size_t i = 1; i <= 2 * MAX_SIZE; ++i)
    {
        if (i % 3 == 0)
        {
            TestNode node;
            node.base = i;
            node.a = i;
            node.b = rand_r(&seed);
            node.c = node.a + node.b;
            node.d = 0;
            seed = node.b;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node), true) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(node.c);
        }
        else
        {
            BaseNode node;
            node.base = i;
            if (ring_buf.push(reinterpret_cast<uint8_t*>(&node), sizeof(node), true) == false)
            {
                max_num = i - 1;
                break;
            }
            index_2_node.push_back(0);
        }
    }

    size_t num = ring_buf.get_num();
    size_t begin_index = index_2_node.size() - num;
    for (size_t i = 1; i <= num; ++i)
    {
        size_t len = 0;
        auto data = ring_buf.front(len, i - 1);
        if ((i + begin_index) % 3 == 0)
        {
            EXPECT_EQ(len, sizeof(TestNode));
            const TestNode* node = reinterpret_cast<const TestNode*>(data);
            EXPECT_EQ(node->base, node->a);
            EXPECT_EQ(node->base, i + begin_index);
            EXPECT_EQ(index_2_node[begin_index + i - 1], node->c);
        }
        else
        {
            EXPECT_EQ(len, sizeof(BaseNode));
            const BaseNode* node = reinterpret_cast<const BaseNode*>(data);
            EXPECT_EQ(node->base, i + begin_index);
        }
    }

    UnfixedRingBuf<> reinit_ring_buf;
    ASSERT_TRUE(reinit_ring_buf.init(p, MAX_SIZE, true));
    EXPECT_EQ(reinit_ring_buf.get_num(), num);

    for (size_t i = 1; i <= num; ++i)
    {
        size_t len = 0;
        auto data = reinit_ring_buf.front(len, i - 1);
        if ((i + begin_index) % 3 == 0)
        {
            EXPECT_EQ(len, sizeof(TestNode));
            const TestNode* node = reinterpret_cast<const TestNode*>(data);
            EXPECT_EQ(node->base, node->a);
            EXPECT_EQ(node->base, i + begin_index);
            EXPECT_EQ(index_2_node[begin_index + i - 1], node->c);
        }
        else
        {
            EXPECT_EQ(len, sizeof(BaseNode));
            const BaseNode* node = reinterpret_cast<const BaseNode*>(data);
            EXPECT_EQ(node->base, i + begin_index);
        }
    }
}

#endif
