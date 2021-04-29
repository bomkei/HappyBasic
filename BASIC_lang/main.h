#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// ----------------------------------------------- //
//  文字列置き換え
// 
//  str       = 置き換えを実行する文字列
//  find      = 検索する文字列
//  replace   = 新しく置き換える文字列
// 
//  戻り      = すべての find が replace として置き換えられた文字列
// 
//  注意   第一引数に渡した文字列は変更されます
// ----------------------------------------------- //
template <class STR>
auto& string_replace(STR& str, STR find, STR replace)
{
  for( int i = 0; i < str.length() - find.length(); )
  {
    // find と一致したら
    if( str.substr(i, find.length()) == find )
    {
      str.erase(i, find.length());  // 元の文字列を削除
      str.insert(i, replace);       // 新しい文字列を挿入
      i += replace.length();        // 操作位置を、挿入した文字列の長さの分進める
    }
    else
      i++;
  }

  return str;
}

// ファイル名の部分のみ取得する
inline auto GetFileName(const char* s)
{
  auto i = strlen(s);

  while( s[i] != '\\' )
    i--;

  return s + i + 1;
}

#define  alart fprintf(stderr,"\t%s:%d\n",GetFileName(__FILE__),__LINE__)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;


//
// オブジェクト
// 即値や変数、文字列などとして使用する
// さらに、動的な型変更もこれにより実現されている
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


//
// トークン
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



Object Expr();

std::vector<Token> Tokenize(std::string&& source);

