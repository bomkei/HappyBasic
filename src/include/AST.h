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
      Shift,
      Bigger,
      BiggerOrEqual,
      Equal,
      NotEqual,


      Immidiate,
      Variable,
      Callfunc,
      Array
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

    void Optimize();

    static Expr* FromInt(int v)
    {
      auto x = new Expr;
      x->token = new Token;
      x->token->obj.v_int = 1;
      return x;
    }
  };

  class Array : public Expr
  {
  public:
    std::vector<Expr*> elems;

    Array()
    {
      type = Type::Array;
    }
  };

  class Callfunc : public Expr
  {
  public:
    std::vector<Expr*> args;

    Callfunc()
    {
      type = Type::Callfunc;
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
    };

    Type type;
    Token* token;

    Stmt(Type type = Block)
      :type(type), token(nullptr)
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

std::ostream& operator << (std::ostream& ss, const AST::Expr& expr);

