/*
 * * file name: string_util.h
 * * description: ...
 * * author: snow
 * * create time:2018  1 12
 * */

#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <string>
using std::string;

namespace pepper
{
extern string trim_str(const string& original_str_, const string& trim_str_, bool is_left_ = true);
extern string string_to_token(const string& str_, const string& delimiter_, string& rest_);

}  // namespace pepper

#endif
