/*
 * * file name: string_utils.cpp
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 12
 * */

#include "utils/string_utils.h"
#include <algorithm>
#include <string>

using std::string;

namespace Pepper
{
static string left_trim(const string& original_str_, const string& trim_str_)
{
    string::size_type pos = original_str_.find_first_not_of(trim_str_);
    if (pos == string::npos)
        return "";
    string::size_type count = original_str_.size() - pos;
    return original_str_.substr(pos, count);
}

static string right_trim(const string& original_str_, const string& trim_str_)
{
    string::size_type pos = original_str_.find_last_not_of(trim_str_);
    if (pos == string::npos)
        return "";
    string::size_type count = pos + 1;
    return original_str_.substr(0, count);
}

string trim_str(const string& original_str_, const string& trim_str_, bool is_left_)
{
    if (is_left_)
        return left_trim(original_str_, trim_str_);
    else
        return right_trim(original_str_, trim_str_);
}

string string_to_token(const string& str_, const string& delimiter_, string& rest_)
{
    string::const_iterator str_beg = str_.begin(), str_end = str_.end();
    string::const_iterator deli_beg = delimiter_.begin(), deli_end = delimiter_.end();
    for (; str_beg != str_end;)
    {
        string::const_iterator first_deli_pos = find_first_of(str_beg, str_end, deli_beg, deli_end);
        string::const_iterator last_deli_pos = first_deli_pos;
        for (; last_deli_pos != str_end; ++last_deli_pos)
            if (count(deli_beg, deli_end, *last_deli_pos) == 0)
                break;
        if (first_deli_pos != str_beg)
        {
            rest_ = str_.substr(distance(str_.begin(), last_deli_pos), distance(last_deli_pos, str_end));
            return str_.substr(distance(str_.begin(), str_beg), distance(str_beg, first_deli_pos));
        }
        str_beg = last_deli_pos;
    }
    return "";
}

}  // namespace Pepper
