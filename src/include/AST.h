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
      Mod,
      Shift,
      Bigger,
      BiggerOrEqual,
      Equal,
      NotEqual,
      LogAND,
      LogOR,

      Assign,

      Immidiate,
      Variable,
      Callfunc,
      Array,
      IndexRef,
      Param,
      
      New,
      MemberAccess,
      MemberVariable
    };

    Type type;
    Expr* left;
    Expr* right;
    Token* token;

    int varIndex = 0;

    Expr(Type type = Immidiate);
    Expr(Type type, Expr* left, Expr* right, Token* tok);
    Expr(float flt);
    Expr(int _int);

    void Optimize();
    std::string ToString() const;
    bool IsConstexpr() const;

    bool equal(AST::Expr const&) const;

    static Expr* FromInt(int v)
    {
      auto x = new Expr;
      x->token = new Token;
      x->token->obj.v_int = 1;
      return x;
    }
    
    static Expr* FromName(std::string const& name) {
      auto x = new Expr;
      x->type = Type::Variable;
      x->token = new Token;
      x->token->obj.name = name;
      return x;
    }

    void clear();
    void fix();
    bool isPrimary();
    bool isBinary();

    AST::Expr& _oprator(Type type, AST::Expr&);
    AST::Expr& operator+=(AST::Expr&);
    AST::Expr& operator-=(AST::Expr&);
    AST::Expr& operator*=(AST::Expr&);
    AST::Expr& operator/=(AST::Expr&);
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

  //
  // Statement
  class Stmt
  {
public:
    enum Type
    {
      If,
      For,
      While,
      Var,
      Block,
      Function,
      Break,
      Continue,
      Return,
      Class,
      Default
    };

    Type type = Default;
    Token* token = nullptr;
    Expr* expr = nullptr;

    std::string ToString(int tab = 0) const;
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
    AST::Block* code;

    While()
    {
      type = Type::While;
    }
  };

  class Function : public Stmt
  {
public:
    std::string name;
    std::vector<Expr*> args;
    AST::Block* code;

    Function()
    {
      type = Type::Function;
    }
  };

  class Class : public Stmt
  {
public:
    std::string name;
    std::vector<Stmt*> member_list;

    Class()
    {
      type = Type::Class;
    }
  };

}

std::ostream& operator<<(std::ostream& ss, const AST::Expr& expr);
