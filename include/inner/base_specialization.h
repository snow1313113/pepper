/*
 * * file name: base_specialization.h
 * * description: ...
 * * author: snow
 * * create time:2018  8 11
 * */

#ifndef _BASE_SPECIALIZATION_H_
#define _BASE_SPECIALIZATION_H_

#include <functional>
#include "../utils/traits_utils.h"
namespace std
{
template <typename T1, typename T2>
struct hash<std::pair<T1, T2>>
{
    size_t operator()(const std::pair<T1, T2>& t_) const { return hash<T1>{}(t_.first); }
};

}  // namespace std

namespace pepper
{
template <typename T1, typename T2>
struct IsEqual<std::pair<T1, T2>>
{
    using T = std::pair<T1, T2>;
    bool operator()(const T& x_, const T& y_) const { return IsEqual<T1>()(x_.first, y_.first); }
};

}  // namespace pepper

#endif
