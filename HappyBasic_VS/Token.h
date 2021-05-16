#pragma once

struct Token
{
  enum Type
  {
    Number,
    String,
    Char,
    Ident,
    Operator,

    End
  };

  Type type = Number;
  std::string str;

  Object obj;
  size_t srcpos = 0;

  Token* next = nullptr;
};