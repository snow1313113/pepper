/*
 * * file name: algorithm_util.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 12
 * */

namespace Pepper
{

/**
    \brief find out the t_ in the x_
    \return return the index of the first t_ in the x_ or end_ + 1 if not found
*/
template<typename T>
extern size_t binary_search_first(const T * x_, size_t begin_, size_t end_, const T & t_)
{
    size_t l = begin_;
    size_t h = end_;
    while(l < h)
    {
        size_t mid = (l + h)/2;
        if(x_[mid] < t_)
            l = mid + 1;
        else
            h = mid;
    }
    if(x_[h] == t_)
        return h;
    else
        return end_ + 1;
}

/**
    \brief find out the t_ in the x_
    \return return the index of the last t_ in the x_ or end_ + 1 if not found
*/
template<typename T>
extern size_t binary_search_last(const T * x_, size_t begin_, size_t end_, const T & t_)
{
    size_t l = begin_;
    size_t h = end_;
    while(l < h)
    {
        size_t mid = (l + h)/2 + 1;
        if(x_[mid] > t_)
            h = mid - 1;
        else
            l = mid;
    }
    if(x_[h] == t_)
        return h;
    else
        return end_ + 1;
}

}
