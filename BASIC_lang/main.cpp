#include "main.h"

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
    //if( is_empty(line) )
    //  continue;

    //while( *line.end() <= ' ' )
    //  line.pop_back();
    //
    //while( *line.begin() <= ' ' )
    //  line.erase(line.begin());

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
  alart;

  auto&& source = std::move(readfile("C:/users/mrzkr/desktop/test.txt"));
  std::cout << source << '\n';

  alart;

  auto tokens = std::move(Tokenize(std::move(source)));

  for( auto&& tok : tokens )
  {
    std::cout << tok.str << '\n';
  }



  alart;
}