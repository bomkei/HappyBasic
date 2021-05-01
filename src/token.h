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

  Token()
  = default;

  Token(Token const& tok);

  Token& operator = (Token const& tok);


};

