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
    
    If()
    {
      type = Type::If;
    }
  };

  class For : public Stmt
  {
  public:
    Expr* begin;
    Expr* end;
    Stmt* code;
    
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
    Expr* expr;
  };

  class Instruction : public Stmt
  {
  public:
    std::string name;
    std::vector<AST::Stmt*> args;
  };


}