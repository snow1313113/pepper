/*
 * * file name: string_util.h
 * * description: ...
 * * author: lemonxu
 * * create time:2018  1 12
 * */

#include <string>
using std::string;

namespace Pepper
{

extern string trim_str(const string & original_str_, const string & trim_str_, bool is_left_ = true);
extern string string_to_token(const string & str_, const string & delimiter_, string & rest_);

}
