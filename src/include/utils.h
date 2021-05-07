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
  template <class T>
  auto Random(T max)
  {
    return rand() % max;
  }

  template <class T>
  auto Random(T begin, T end)
  {
    return rand() % (end - begin);
  }

  template <class T>
  auto GetRandomStr(auto len = 20)
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
  }
};