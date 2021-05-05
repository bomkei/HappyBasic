#pragma once

class Optimizer
{
public:
  
  struct Term
  {
    enum Sign
    {
      Plus,
      Minus
    };

    Sign sign = Plus;
    AST::Expr* term;

    Term(AST::Expr*);
  };

  static void EvalConstexpr(AST::Expr* expr);
  static std::vector<Term> GetTermsFromExpr(AST::Expr* expr);



};