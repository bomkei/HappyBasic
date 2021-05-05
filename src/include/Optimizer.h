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

    Sign sign;
    AST::Expr* term;

    Term(AST::Expr*);
  };

  static bool ReduceFactors(AST::Expr* expr);
  static std::vector<Term> GetTermsFromExpr(AST::Expr* expr);


};