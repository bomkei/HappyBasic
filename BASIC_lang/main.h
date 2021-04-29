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

  Object(Type type = Int)
    :type(type), v_int(0), v_float(0), v_char(0)
  {

  }

  Object(Object const& obj)
  {
    *this = obj;
  }

  Object(Object&& obj)
  {
    type = obj.type;
    v_int = obj.v_int;
    v_float = obj.v_float;
    v_char = obj.v_char;

    name = std::move(obj.name);
    list = std::move(obj.list);

    Object::Clear(obj);
  }

  Object& operator = (Object const& obj)
  {
    auto na = this->name;

    *this = obj;
    
    this->name = na;
    return *this;
  }

  static void Clear(Object& obj)
  {
    obj.type = Int;
    obj.v_int = obj.v_float = obj.v_char = 0;
    obj.list.clear();
    obj.name.clear();
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

  Type type = Number;
  std::string str;

  Object obj;
  
  Token()
  {

  }

  Token(Token const& tok)
  {
    *this = tok;
  }

  Token(Token&& tok)
  {
    type = tok.type;

  }

  Token& operator = (Token const& tok)
  {
    type = tok.type;
    str = tok.str;
    obj = tok.obj;

    return *this;
  }
};


//
// �m�[�h
// �\���؂���������
struct Node
{
  enum Type
  {
    Add,
    Sub,
    Mul,
    Div,

    Immidiate,
    Callfunc
  };

  Type type;
  Node* lhs;
  Node* rhs;
  Token tok;

  i64 varIndex;
  std::vector<Node*> list;

  Node(Type type = Immidiate)
    :type(type), lhs(nullptr), rhs(nullptr), varIndex(0)
  {

  }

  Node(Type type, Node* lhs, Node* rhs, Token const& tok)
    :Node(type)
  {
    this->lhs = lhs;
    this->rhs = rhs;
    this->tok = tok;
  }
};


Object Expr();

std::vector<Token> Tokenize(std::string&& source);

