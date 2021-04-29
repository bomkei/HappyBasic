#include "main.h"

namespace
{
  std::string source;
  size_t srcpos;

  auto op_tokens =
  {
    "...",
    "&=",
    "|=",
    "^=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    ">>",
    "<<",
    "==",
    "!=",
    ">=",
    "<=",
    "&&",
    "||",
    "!",
    "~",
    "&",
    "|",
    ">",
    "<",
    "=",
    "+",
    "-",
    "*",
    "/",
    "%",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    ",",
    ".",
    ";",
    ":",
    "\n",
  };
}

std::vector<Token> Tokenize(std::string&& src)
{
  source = std::move(src);
  srcpos = 0;

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
    while( peek() != '\n' && peek() <= ' ' )
      next();
  };

  static auto isident = [](char c)
  {
    return isalnum(c) || c == '_';
  };

  pass_space();

  while( check() )
  {
    auto c = peek();
    auto pos = srcpos;

    Token tok;
    tok.srcpos = pos;

    if( isdigit(c) )
    {
      tok.type = Token::Number;

      while( check() && isdigit(c = peek()) )
        tok.str += c, next();
    }
    else if( isident(c) )
    {
      tok.type = Token::Ident;

      while( check() && isident(c = peek()) )
        tok.str += c, next();
    }
    else if( c == '"' )
    {
      tok.type = Token::String;

      next();

      while( check() && (c = peek()) != '"' )
        tok.str += c, next();

      next();
    }
    else
    {
      auto hit = false;
      tok.type = Token::Operator;

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
        tok.Error("unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  return tokens;
}

