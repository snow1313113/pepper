#include <vector>
#include <iostream>
#include "../../include/hash_mem_pool.h"
using namespace std;
using namespace Pepper;

struct BaseNode
{
    uint32_t base;
};

struct TestNode : public BaseNode
{
    uint32_t a;
    uint32_t b;
    size_t c;
    char d;
};

int main()
{
    size_t max_num = 1451;
    uint32_t bucket_num = NearbyPrime(max_num);
    size_t mem_size = HashMap::GetMemSize(max_num, bucket_num);
    uint8_t* mem = new uint8_t[mem_size];
    HashMap mem_pool;

    bool result = mem_pool.Init(mem, max_num, bucket_num, mem_size);
    ASSERT_TRUE(result);
    ASSERT_TRUE(mem_pool.IsEmpty());
    ASSERT_FALSE(mem_pool.IsFull());
    EXPECT_EQ(mem_pool.Capacity(), max_num);
    EXPECT_EQ(mem_pool.Size(), 0);

    // 记录下每个node的ref，后面测试Deref转回来的结果是否正确
    map<size_t, size_t> index_map;
    // 记录一批要del的node，后面做删除测试用
    set<size_t> del_set;
    // 每个节点的random值，后面校验测试用
    map<size_t, size_t> node_rand_map;
    TestNode node;
    uint32_t seed = max_num;
    for (size_t i = 1; i < max_num + 1; ++i)
    {
        auto result_pair = mem_pool.Insert(i);
        ASSERT_TRUE(result_pair.second);
        EXPECT_NE(result_pair.first, mem_pool.End());

        auto p = &(result_pair.first->second);
        p->a = i;
        p->b = rand_r(&seed);
        p->c = p->a + p->b;
        p->d = 0;
        seed = p->b;

        size_t index = mem_pool.Ref(&(*result_pair.first));
        EXPECT_NE(index, 0);

        auto ret_pair = index_map.insert(std::make_pair(index, i));
        ASSERT_TRUE(ret_pair.second);

        auto ret2_pair = node_rand_map.insert(std::make_pair(i, p->b));
        ASSERT_TRUE(ret2_pair.second);

        if (p->b % 3 == 0)
            del_set.insert(i);
    }

    // 如果真的找不到一个了，就找第一个了
    if (del_set.empty())
        del_set.insert(1);

    EXPECT_EQ(mem_pool.Size(), max_num);
    ASSERT_TRUE(mem_pool.IsFull());
    EXPECT_EQ(mem_pool.Size(), max_num);

    for (size_t i = 1; i < max_num + 1; ++i)
    {
        auto iter = mem_pool.Find(i);
        EXPECT_NE(iter, mem_pool.End());

        auto it = node_rand_map.find(i);
        EXPECT_NE(it, node_rand_map.end());
        EXPECT_EQ(iter->second.b, it->second);
    }

    // 记录下每个node的key，后面测试是否node都没错
    set<uint32_t> key_set;
    size_t count = 0;
    for (auto beg = mem_pool.Begin(), end = mem_pool.End(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        auto ret_pair = key_set.insert(beg->first);
        ASSERT_TRUE(ret_pair.second);
        ++count;
    }
    EXPECT_EQ(count, max_num);

    for (size_t i = 1; i < max_num + 1; ++i)
    {
        EXPECT_NE(key_set.find(i), key_set.end());
    }

    // 测试ref和deref
    for (auto it : index_map)
    {
        size_t index = it.first;
        auto p = mem_pool.Deref(index);
        EXPECT_NE(p, nullptr);
        EXPECT_EQ(p->second.a, it.second);
    }

    // 删除部分元素后再测试
    for (auto it : del_set)
    {
        key_set.erase(it);
        ASSERT_TRUE(mem_pool.Erase(it));
    }

    ASSERT_FALSE(mem_pool.IsFull());
    EXPECT_EQ(mem_pool.Capacity(), max_num);
    EXPECT_EQ(mem_pool.Size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.Size(), key_set.size());

    count = 0;
    for (auto beg = mem_pool.Begin(), end = mem_pool.End(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.Size(), count);

    // 测试init的check_header参数
    result = mem_pool.Init(mem, max_num, bucket_num, mem_size, true);
    ASSERT_TRUE(result);

    ASSERT_FALSE(mem_pool.IsFull());
    EXPECT_EQ(mem_pool.Capacity(), max_num);
    EXPECT_EQ(mem_pool.Size(), max_num - del_set.size());
    EXPECT_EQ(mem_pool.Size(), key_set.size());

    // 重新看看每个node对不对
    count = 0;
    for (auto beg = mem_pool.Begin(), end = mem_pool.End(); beg != end; ++beg)
    {
        EXPECT_EQ(beg->first, (*beg).second.a);
        EXPECT_EQ((*beg).second.a + beg->second.b, beg->second.c);
        EXPECT_EQ((*beg).second.d, 0);
        EXPECT_NE(key_set.find(beg->second.a), key_set.end());
        ++count;
    }
    EXPECT_EQ(mem_pool.Size(), count);

    // 测试清空
    mem_pool.Clear();

    ASSERT_TRUE(mem_pool.IsEmpty());
    ASSERT_FALSE(mem_pool.IsFull());
    EXPECT_EQ(mem_pool.Capacity(), max_num);
    EXPECT_EQ(mem_pool.Size(), 0);

    return 0;
}


