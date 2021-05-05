#include "main.h"

Optimizer::Term::Term(AST::Expr* term)
  :term(term)
{

}

std::vector<Optimizer::Term> Optimizer::GetTermsFromExpr(AST::Expr* expr)
{
  std::vector<Term> terms;

  if( expr->type != AST::Expr::Add && expr->type != AST::Expr::Sub )
  {
    terms.push_back(expr);
    return terms;
  }

  auto left = GetTermsFromExpr(expr->left);
  auto right = GetTermsFromExpr(expr->right);

  right[0].sign = expr->type == AST::Expr::Add ? Term::Sign::Plus : Term::Sign::Minus;

  for( auto&& t : left )
    terms.emplace_back(t);

  for( auto&& t : right )
    terms.emplace_back(t);

  return terms;
}