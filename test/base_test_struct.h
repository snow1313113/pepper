/*
 * * file name: base_test_struct.h
 * * description: ...
 * * author: snow
 * * create time:2019  6 01
 * */

#ifndef _BASE_TEST_STRUCT_H_
#define _BASE_TEST_STRUCT_H_

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

#endif
