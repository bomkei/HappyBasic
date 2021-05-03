#pragma once


namespace AST
{

  class Expr
  {
  public:
    enum Type
    {
      Add,
      Sub,
      Mul,
      Div,
      Immidiate
    };

    Type type = Immidiate;
    Expr* left = nullptr;
    Expr* right = nullptr;
    Token* token = nullptr;

    Expr(Type type = Immidiate)
      :type(type)
    {
    }

    Expr(Type type, Expr* left, Expr* right, Token* tok)
      :type(type), left(left), right(right), token(tok)
    {
    }
  };

  class Stmt
  {
  public:
    enum Type
    {
      If,
      For,
      While,
      Block,
      Default
    };

    Type type = If;
    Expr* expr = nullptr;
    std::vector<Stmt*> list;

    Stmt(Type type = Block)
      :type(type)
    {
    }
  };

  class If : public Stmt
  {
  public:
    Expr* cond;
    Stmt* if_true;
    Stmt* if_false;
  };

  class For : public Stmt
  {
  public:
    Expr* begin;
    Expr* end;
    Stmt* code;
  };

  class While : public Stmt
  {
  public:
    Expr* cond;
    Stmt* code;
  };



}