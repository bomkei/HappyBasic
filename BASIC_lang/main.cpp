#include "main.h"

auto& string_replace(std::string& str, std::string const& find, std::string const& replace)
{
  for( int i = 0; i < str.length() - find.length(); )
  {
    if( str.substr(i, find.length()) == find )
    {
      str.erase(i, find.length());
      str.insert(i, replace);
      i += replace.length();
    }
    else
      i++;
  }

  return str;
}

auto readfile(std::string const& path)
{
  static auto is_empty = [](std::string const& str)
  {
    for( auto&& c : str )
      if( c > ' ' ) return false;

    return true;
  };

  std::ifstream ifs(path);
  std::string ret, line;

  if( ifs.fail() )
  {
    std::cout << "cannot open file";
    exit(1);
  }

  while( std::getline(ifs, line) )
  {
    if( is_empty(line) )
      continue;

    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    ret += line + '\n';
  }

  string_replace(ret, "\\\n", "");

  if( is_empty(ret) )
  {
    std::cout << "empty source file";
    exit(1);
  }

  return ret;
}

void RunProgram()
{

}

int main()
{
  auto source = std::move(readfile("C:/users/mrzkr/desktop/test.txt"));
  std::cout << source << '\n';

  auto tokens = std::move(Tokenize(std::move(source)));

  for( auto&& tok : tokens )
  {
    std::cout << ":" << tok.str << '\n';
  }



}


