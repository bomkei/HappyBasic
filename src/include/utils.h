#pragma once

#if _DEBUG_BUILD_
#define  alart  fprintf(stderr,"\t%s:%d\n",get_file_name(__FILE__),__LINE__)
#define  view_pointer(p)  fprintf(stderr,"\t%s = %p\n", #p, p)
#else
#define alart
#define view_pointer(p) p
#endif

inline auto get_file_name(const char* s)
{
  auto i = strlen(s) - 1;
  while( s[i] != '\\' && s[i] != '/' ) i--;
  return s + i + 1;
}

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

namespace Utils
{
  template <class T, class U, class Count>
  auto ToVector(Count count, U** items)
  {
    std::vector<T> ret;

    for( size_t i = 0; i < count; i++ )
    {
      ret.emplace_back(T(items[i]));
    }

    return ret;
  }

  template <class T>
  T Random(T max)
  {
    return rand() % max;
  }

  template <class T>
  T Random(T begin, T end)
  {
    return begin + rand() % (end - begin);
  }

  template <class T>
  T GetRandomStr(int len = 20)
  {
    T str;

    for( int i = 0; i < len; i++ )
    {
      if( Random(0, len) == 0 )
      {
        str += '_';
        continue;
      }

      switch( Random<int>(3) )
      {
        case 0:
          str += Random('a', 'z');
          break;

        case 1:
          str += Random('A', 'Z');
          break;

        case 2:
          str += Random('0', '9');
          break;
      }
    }

    return str;
  }
};