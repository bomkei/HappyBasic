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
    
    Type    type   = Immidiate;
    Expr*   left   = nullptr;
    Expr*   right  = nullptr;
    Token*  token  = nullptr;
  };
  
  class Stmt
  {
  public:
    enum Type
    {
      If,
      For,
      While
    };
    
    Type    type   = If;
    std::vector<Stmt*> list;
  };
  
  class If : public Stmt
  {
  public:
    Expr*   cond;
    Stmt*   if_true;
    Stmt*   if_false;
  };
  
  class For : public Stmt
  {
  public:
    Expr*   begin;
    Expr*   end;
    Stmt*   code;
  };
  
  class While : public Stmt
  {
  public:
    Expr*   cond;
    Stmt*   code;
  };
  
  
  
}