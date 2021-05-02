#pragma once

//
// トークン
struct Token
{
  enum Type
  {
    Number,
    String,
    Char,
    Ident,
    Operator
  };

  Type type = Number;
  std::string str;

  Object obj;
  size_t srcpos = 0;
};


[[noreturn]]
void Error(Token const& tok, std::string const& msg)
{
  throw std::make_pair(tok, msg);
}

