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
void Error(size_t errpos, std::string const& msg)
{
  std::cout << errpos << ": " << msg;
  exit(1);
}

