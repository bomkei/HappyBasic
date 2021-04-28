#include "main.h"

namespace
{
  std::vector<Token> g_tokens;
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

  if( ifs.is_open() == false )
  {
    std::cout << "cannot open file";
    exit(1);
  }

  while( std::getline(ifs, line) )
  {
    if( is_empty(line) )
      continue;

    while( *line.end() <= ' ' )
      line.pop_back();
    
    while( *line.begin() <= ' ' )
      line.erase(line.begin());

    ret += line + '\n';
  }

  if( ret.empty() )
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
  auto&& source = std::move(readfile("C:/users/mrzkr/desktop/test.txt"));


}