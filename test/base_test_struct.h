/*
 * * file name: base_test_struct.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 01
 * */

#ifndef _BASE_TEST_STRUCT_H_
#define _BASE_TEST_STRUCT_H_

#include "utils/traits_utils.h"

struct BaseNode
{
    uint32_t key;
    uint32_t base;
};

struct TestNode : public BaseNode
{
    uint32_t a;
    uint32_t b;
    size_t c;
    char d;
};

namespace std
{
template <>
struct hash<BaseNode>
{
    size_t operator()(const BaseNode &t_) const { return hash<uint32_t>{}(t_.key); }
};

template <>
struct hash<TestNode>
{
    size_t operator()(const TestNode &t_) const { return hash<uint32_t>{}(t_.a); }
};
}  // namespace std

namespace pepper
{
template <>
struct IsEqual<BaseNode>
{
    bool operator()(const BaseNode &x, const BaseNode &y) const { return x.key == y.key; }
};

template <>
struct IsEqual<TestNode>
{
    bool operator()(const TestNode &x, const TestNode &y) const { return x.a == y.a; }
};
}  // namespace pepper

#endif
