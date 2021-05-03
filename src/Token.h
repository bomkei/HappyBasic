#pragma once

//
// ƒg[ƒNƒ“
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

