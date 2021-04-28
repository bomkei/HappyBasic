#include "main.h"

std::vector<Token> Tokenize(std::string& source)
{
  static size_t pos = 0;

  static auto peek = [&]
  {
    return source[pos];
  };

  static auto check = [&]
  {
    return pos < source.length();
  };

  static auto next = [&](int n = 1)
  {
    pos += n;
  };

  static auto pass_space = [&]
  {
    while( peek() <= ' ' )
      next();
  };

  std::vector<Token> tokens;
  Token tok;

  pass_space();

  while( check() )
  {
    auto c = peek();

    if( isdigit(c) )
    {

    }
  }

  return tokens;
}

