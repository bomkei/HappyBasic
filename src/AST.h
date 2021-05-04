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
      Immidiate,
      Variable
    };

    Type type = Immidiate;
    Expr* left = nullptr;
    Expr* right = nullptr;
    Token* token = nullptr;

    int varIndex = 0;

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
      Assign,
      Instruction,
      Block,

      None
    };

    Type type;
    Expr* expr;
    Token* token;

    Stmt(Type type = None)
      :type(type), expr(nullptr), token(nullptr)
    {
    }
  };

  class Block : public Stmt
  {
  public:
    std::vector<Stmt*> list;

    Block()
    {
      type = Type::Block;
    }
  };

  class If : public Stmt
  {
  public:
    using Pair = std::pair<AST::Expr*, AST::Block*>;

    std::vector<Pair> pairs;

    If()
    {
      type = Type::If;
    }
  };

  class For : public Stmt
  {
  public:
    Expr* counter;
    Expr* begin;
    Expr* end;
    AST::Block* code;

    For()
    {
      type = Type::For;
    }
  };

  class While : public Stmt
  {
  public:
    Expr* cond;
    Stmt* code;

    While()
    {
      type = Type::While;
    }
  };

  class Assign : public Stmt
  {
  public:
    Expr* var;
    Expr* value;

    Assign()
    {
      type = Type::Assign;
    }
  };

  class Instruction : public Stmt
  {
  public:
    std::string name;
    std::vector<AST::Expr*> args;

    Instruction()
    {
      type = Type::Instruction;
    }
  };


}

