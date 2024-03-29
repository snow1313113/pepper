/*
 * * file name: base_struct.h
 * * description: ...
 * * author: snow
 * * create time:2018  1 12
 * */
#ifndef _BASE_STRUCT_H_
#define _BASE_STRUCT_H_

namespace pepper
{
template <typename T>
struct Singleton
{
    static T &instance()
    {
        static T only;
        return only;
    }

protected:
    ~Singleton() {}
};

template <typename T>
class AutoRes
{
public:
    typedef void (T::*T_F)();
    AutoRes(T *p_, T_F f_) : m_ptr(p_), m_fun(f_) {}
    ~AutoRes()
    {
        if (m_ptr)
            (m_ptr->*m_fun)();
    }
    T *it() { return m_ptr; }
    void throw_away() { m_ptr = 0; }

private:
    AutoRes(const AutoRes &) = delete;
    AutoRes &operator=(const AutoRes &) = delete;

private:
    T *m_ptr;
    T_F m_fun;
};

template <typename T>
struct Link
{
    T prev = 0;
    T next = 0;
};

template <typename T1, typename T2>
struct Pair
{
    T1 first;
    T2 second;
};

}  // namespace pepper

#endif
