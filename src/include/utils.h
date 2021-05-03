#pragma once

template <class T, class F, class... Args>
int find_vector(std::vector<T>& vec, F compare, Args ...args)
{
  for( int i = 0; i < vec.size(); i++ )
    if( compare(vec[i], args...) )
      return i;

  return -1;
}

template <class... Args>
std::string format(std::string const& fmt, Args... args)
{
  char buf[1000];
  sprintf(buf, fmt.c_str(), args...);
  return buf;
}

