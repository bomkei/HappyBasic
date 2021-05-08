#pragma once

namespace Optimizer
{
  struct Term
  {
    enum Sign
    {
      Plus,
      Minus
    };

    Sign sign;
    AST::Expr* term;

    Term(AST::Expr* t)
      :sign(Sign::Plus), term(t)
    {
      
    }
  };


  std::vector<Term> GetTerms(AST::Expr* expr);

  std::vector<std::string> GetAlphabets(AST::Expr*);



}