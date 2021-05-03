#pragma once

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct Token;
using ErrorInfo = std::pair<Token, std::string>;


template <class T, class F, class... Args>
int find_vector(std::vector<T>& vec, F compare, Args ...args)
{
  for( int i = 0; i < vec.size(); i++ )
    if( compare(vec[i], args...) )
      return i;

  return -1;
}
