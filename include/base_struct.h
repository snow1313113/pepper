/*
 * * file name: base_struct.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 12
 * */

namespace Pepper
{

template<typename T>
struct Singleton
{
    static T & instance()
    {
        static T only;
        return only;
    }
protected:
    ~Singleton(){}
};

template<typename T>
class AutoRes
{
public:
    typedef void(T::*T_F)();
    AutoRes(T * p_, T_F f_):m_ptr(p_),m_fun(f_){}
    ~AutoRes()
    {
        if(m_ptr)
            (m_ptr->*m_fun)();
    }
    T * it(){ return m_ptr; }
    void throw_away(){ m_ptr = 0; }
private:
    AutoRes(const AutoRes &) = delete;
    AutoRes & operator=(const AutoRes &) = delete;
private:
    T * m_ptr;
    T_F m_fun;
};

template<typename T>
struct Link
{
    T prev = 0;
    T next = 0;
};


}
