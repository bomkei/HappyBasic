#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// ----------------------------------------------- //
//  ������u������
// 
//  str       = �u�����������s���镶����
//  find      = �������镶����
//  replace   = �V�����u�������镶����
// 
//  �߂�      = ���ׂĂ� find �� replace �Ƃ��Ēu��������ꂽ������
// 
//  ����   �������ɓn���ꂽ�������ύX���A�߂�l�Ƃ��ĕԂ��܂�
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


//
// vector ����
template <class T, class... Args>
i64 find_vector(std::vector<T> const& vec, Args ...args)
{
  for( i64 i = 0; i < vec.size(); i++ )
    if( vec[i] == T(args...) )
      return i;

  return -1;
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
    Array,
    Pointer
  };

  Type type;
  std::string name;

  int v_int;
  float v_float;
  char v_char;
  std::vector<Object> list;

  Object* var_ptr;

  Object(Type type = Int);

  Object& operator = (Object const& obj);

  std::string to_string() const;

  static void Clear(Object& obj);

  bool is_string() const;
  bool eval() const;
};

//
// �g�[�N��
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
  {
  }

  Token(Token const& tok);

  Token& operator = (Token const& tok);

  [[noreturn]]
  void Error(std::string const& msg);
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
    Mod,
    Div,

    Shift,
    Bigger,
    Equal,
    NotEqual,

    Assign,

    Block,
    If,
    For,
    While,

    Break,
    Continue,

    Array,
    IndexRef,
    Reference,
    Immidiate,
    Variable,
    Instruction,
    Callfunc,
    MemberAccess
  };

  Type type;
  Node* lhs;
  Node* rhs;
  Token tok;

  i64 varIndex;
  std::vector<Node*> list;

  Node(Type type = Immidiate);
  Node(Type type, Node* lhs, Node* rhs, Token* tok = nullptr);

  static Node* FromInt(int v)
  {
    auto n = new Node;
    n->tok.obj.v_int = v;
    return n;
  }
};




std::vector<Token> Tokenize(std::string&& source);

Node* Parse(std::vector<Token>&& tokens);

Object Callfunc(Node* node);
Object RunExpr(Node* node);
Object RunStmt(Node* node);

bool is_hex(std::string);

void SetConsoleColor(int col);
