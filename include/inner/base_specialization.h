/*
 * * file name: base_specialization.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  8 11
 * */

#ifndef BASE_SPECIALIZATION_H
#define BASE_SPECIALIZATION_H

#include "../utils/traits_utils.h"

namespace Pepper
{

template<typename T1, typename T2>
struct SimpleHash<std::pair<T1, T2> >
{
    size_t operator()(const std::pair<T1, T2> & t_) const
    {
        return SimpleHash<T1>()(t_.first);
    }
};

template <typename T1, typename T2>
struct IsEqual<std::pair<T1, T2> >
{
    typedef std::pair<T1, T2> T;
    bool operator()(const T & x_, const T & y_) const
    {
        return IsEqual<T1>()(x_.first, y_.first);
    }
};

}


#endif
