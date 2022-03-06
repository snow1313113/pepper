/*
 * * file name: policy.h
 * * description: ...
 * * author: snow
 * * create time:2022  3 06
 * */

#ifndef _POLICY_H_
#define _POLICY_H_

namespace pepper
{
namespace inner
{
template <typename T, bool INHERIT = std::is_empty<T>::value && !std::is_final<T>::value>
struct EBOProxy
{
    T& operator*() { return m_obj; }
    T const& operator*() const { return m_obj; }

private:
    static T m_obj;
};

template <typename T>
struct EBOProxy<T, true> : public T
{
    T& operator*() { return *this; }
    T const& operator*() const { return *this; }
};

template <typename HASH, typename IS_EQUAL>
struct Policy : private EBOProxy<HASH>, private EBOProxy<IS_EQUAL>
{
    using Hasher = EBOProxy<HASH>;
    using IsEqual = EBOProxy<IS_EQUAL>;

    HASH& hash() { return *static_cast<Hasher&>(*this); }
    const HASH& hash() const { return *static_cast<const Hasher&>(*this); }

    IS_EQUAL& is_equal() { return *static_cast<IsEqual&>(*this); }
    const IS_EQUAL& is_equal() const { return *static_cast<const IsEqual&>(*this); }
};

}  // namespace inner

template <typename T>
using DefaultPolicy = inner::Policy<std::hash<T>, IsEqual<T>>;

}  // namespace pepper

#endif
