/*
 * * file name: container_utils.h
 * * description: ...
 * * author: snow
 * * create time:2018  1 12
 * */

namespace Pepper
{
template <typename map_type, typename key_arg_type, typename value_arg_type>
inline typename map_type::iterator effective_add_or_update(map_type& m_, const key_arg_type& k_,
                                                           const value_arg_type& v_)
{
    typename map_type::iterator iter = m_.lower_bound(k_);
    if (iter != m_.end() && !(m_.key_comp()(k_, iter->first)))
    {
        iter->second = v_;
        return iter;
    }
    else
    {
        typedef typename map_type::value_type v_type;
        return m_.insert(iter, v_type(k_, v_));
    }
}

}  // namespace Pepper
