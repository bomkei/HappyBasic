#pragma once

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

  Token* next = nullptr;

  Token(Type type, Token* back, size_t pos) {
    type = type;
    srcpos = pos;

    if( back ) {
      back->next = this;
    }
  }
};