#include "main.h"

namespace
{
  std::string source;
  size_t srcpos;

  auto op_tokens =
  {
    "+",
    "-",
    "*",
    "/",
  };
}

void Error(i64 errpos, std::string msg)
{
  
}

std::vector<Token> Tokenize(std::string&& src)
{
  source = std::move(src);

  std::vector<Token> tokens;

  static auto peek = []
  {
    return source[srcpos];
  };

  static auto check = []
  {
    return srcpos < source.length();
  };

  static auto next = [](int n = 1)
  {
    srcpos += n;
  };

  static auto pass_space = []
  {
    while( peek() <= ' ' )
      next();
  };

  pass_space();

  while( check() )
  {
    auto c = peek();
    auto pos = srcpos;

    Token tok;

    alart;

    if( isdigit(c) )
    {
      alart;
      while( check() && isdigit(c = peek()) )
      {
        tok.str += c;
        next();
      }
    }
    else if( c == '"' )
    {
      next();

      while( check() && (c = peek()) != '"' )
        tok.str += c, next();

      next();
    }
    else
    {
      auto hit = false;

      for( std::string str : op_tokens )
      {
        if( srcpos + str.length() <= source.length() && source.substr(srcpos, str.length()) == str )
        {
          next(str.length());
          hit = true;
          tok.str = str;
          break;
        }
      }

      if( hit == false )
      {
        Error(pos, "unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  return tokens;
}

