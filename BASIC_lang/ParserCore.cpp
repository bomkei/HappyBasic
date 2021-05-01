#include "main.h"

std::vector<Object> g_variables;

namespace
{
  std::vector<Token> g_tokens;
  size_t tkIndex;
  Token* csmtok;

  auto& curtok()
  {
    return g_tokens[tkIndex];
  }

  auto next(int n = 1)
  {
    tkIndex += n;
  }

  auto check()
  {
    return tkIndex < g_tokens.size();
  }

  auto consume(std::string const& str)
  {
    if( curtok().str == str )
    {
      csmtok = &curtok();
      next();
      return true;
    }

    return false;
  }

  auto expect(std::string const& str)
  {
    if( consume(str) == false )
    {
      if( str == "\n" )
      {
        tkIndex--;
        curtok().Error("expect new line after this token");
      }
      else
        curtok().Error("expect '" + str + "'");
    }
  }

  i64 find_var(std::string const& name)
  {
    for( size_t i = 0; i < g_variables.size(); i++ )
    {
      if( g_variables[i].name == name )
        return i;
    }

    return -1;
  }
}


