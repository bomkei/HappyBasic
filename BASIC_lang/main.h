#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// ----------------------------------------------- //
//  ������u������
// 
//  str       = �u�����������s���镶����
//  find      = �������镶����
//  replace   = �V�����u�������镶����
// 
//  �߂�      = ���ׂĂ� find �� replace �Ƃ��Ēu��������ꂽ������
// 
//  ����   �������ɓn����������͕ύX����܂�
// ----------------------------------------------- //
template <class STR>
auto& string_replace(STR& str, STR find, STR replace)
{
  for( int i = 0; i < str.length() - find.length(); )
  {
    // find �ƈ�v������
    if( str.substr(i, find.length()) == find )
    {
      str.erase(i, find.length());  // ���̕�������폜
      str.insert(i, replace);       // �V�����������}��
      i += replace.length();        // ����ʒu���A�}������������̒����̕��i�߂�
    }
    else
      i++;
  }

  return str;
}

// �t�@�C�����̕����̂ݎ擾����
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
// �I�u�W�F�N�g
// ���l��ϐ��A������ȂǂƂ��Ďg�p����
// ����ɁA���I�Ȍ^�ύX������ɂ���������Ă���
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
// �g�[�N��
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

