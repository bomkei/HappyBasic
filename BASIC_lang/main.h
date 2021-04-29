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
// �\�[�X�R�[�h
extern std::string g_source;

//
// �ϐ�
struct Object;
extern std::vector<Object> g_variables;


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

  Object* var_ptr;

  Object(Type type = Int)
    :type(type), v_int(0), v_float(0), v_char(0), var_ptr(nullptr)
  {

  }

  Object(Object const& obj)
  {
    *this = obj;
  }

  Object(Object&& obj)
  {
    *this = std::move(obj);
  }

  Object& operator = (Object const& obj)
  {
    type = obj.type;
    v_int = obj.v_int;
    v_float = obj.v_float;
    v_char = obj.v_char;

    name = obj.name;
    list = obj.list;
    var_ptr = obj.var_ptr;

    return *this;
  }
  
  Object& operator = (Object&& obj)
  {
    type = obj.type;
    v_int = obj.v_int;
    v_float = obj.v_float;
    v_char = obj.v_char;

    name = std::move(obj.name);
    list = std::move(obj.list);
    var_ptr = obj.var_ptr;

    Object::Clear(obj);

    return *this;
  }

  std::string to_string() const
  {
    switch( type )
    {
      case Int:
        return std::to_string(v_int);

      case Float:
        return std::to_string(v_float);

      case Char:
        return std::string(1, v_char);

      case Array:
      {
        std::string s = "";

        if( is_string() )
        {
          for( auto&& c : list )
            s += c.v_char;

          return s;
        }

        for( int i = 0; i < list.size(); i++ )
        {
          s += list[i].to_string();
          if( i < list.size() - 1 ) s += ", ";
        }

        return  "[" + s + "]";
      }
    }

    return "";
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
    switch( type )
    {
      case Int: return v_int != 0;
      case Float: return v_float != 0;
      case Char: return v_char != 0;
      case Array: return list.size() != 0;
    }

    return false;
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
  size_t srcpos;

  Token()
  {

  }

  Token(Token const& tok)
  {
    *this = tok;
  }

  Token(Token&& tok)
  {
    *this = std::move(tok);
  }

  Token& operator = (Token const& tok)
  {
    type = tok.type;
    str = tok.str;
    obj = tok.obj;
    srcpos = tok.srcpos;

    return *this;
  }

  Token& operator = (Token&& tok)
  {
    type = tok.type;
    str = std::move(tok.str);
    obj = std::move(tok.obj);
    srcpos = tok.srcpos;

    return *this;
  }
  
  [[noreturn]]
  void Error(std::string const& msg)
  {
    size_t line = 1;
    size_t begin = 0;
    size_t end = g_source.length();

    for( size_t i = 0; i < srcpos; i++ )
    {
      if( g_source[i] == '\n' )
      {
        line++;
        begin = i + 1;
      }
    }

    for( size_t i = srcpos; i < end; i++ )
    {
      if( g_source[i] == '\n' )
      {
        end = i;
        break;
      }
    }

    auto line_str = g_source.substr(begin, end - begin);
    auto pos = srcpos - begin;

    printf("%6zd|%s\n", line, line_str.c_str());
    printf("       %s^%s  %s\n", std::string(pos, ' ').c_str(), std::string(str.length() ? str.length() - 1 : 0 , '~').c_str(), msg.c_str());

    exit(1);
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
    Assign,

    Block,
    If,
    

    Immidiate,
    Instruction,
    Callfunc,
    Variable
  };

  Type type;
  Node* lhs;
  Node* rhs;
  Token tok;

  i64 varIndex;
  std::vector<Node*> list;

  Node(Type type = Immidiate);
  Node(Type type, Node* lhs, Node* rhs);

};




std::vector<Token> Tokenize(std::string&& source);

Node* Parse(std::vector<Token>&& tokens);

Object Callfunc(Node* node);
Object RunExpr(Node* node);
Object RunStmt(Node* node);

