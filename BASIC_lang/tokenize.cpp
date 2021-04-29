#include "main.h"

std::string g_source;
size_t srcpos;

static auto op_tokens =
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

std::vector<Token> Tokenize(std::string&& src)
{
  g_source = std::move(src);
  srcpos = 0;

  std::vector<Token> tokens;

  static auto peek = []
  {
    return g_source[srcpos];
  };

  static auto check = []
  {
    return srcpos < g_source.length();
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
        if( srcpos + str.length() <= g_source.length() && g_source.substr(srcpos, str.length()) == str )
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

