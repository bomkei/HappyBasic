#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Object
{
  enum Type
  {
    Int,
    Float,
    Char,
    Array
  };

  Type type;
  std::string name;

  int v_int;
  float v_float;
  char v_char;
  std::vector<Object> list;

  Object& operator = (Object const& obj)
  {
    auto na = this->name;

    *this = obj;
    
    this->name = na;
    return *this;
  }

  bool is_string() const
  {
    if( this->type != Array )
      return false;

    for( auto&& c : list )
      if( c.type != Char )
        return false;

    return list.size() != 0;
  }

  bool eval() const
  {

  }
};

struct Token
{
  enum Type
  {
    Number,
    String,
    Ident,
    Operator
  };

  Type type;
  std::string str;

  Object obj;
  
};



Object Expr(std::string const& str);

std::vector<Token> Tokenize(std::string& source);

