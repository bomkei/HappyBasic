#include "main.h"

Optimizer::Term::Term(AST::Expr* term)
  :term(term), sign(Sign::Plus)
{

}

void Optimizer::CalcConstexpr(AST::Expr* expr)
{
  switch( expr->type )
  {
    case AST::Expr::Immidiate:
    case AST::Expr::Variable:
      break;

    case AST::Expr::Array:
      for( auto&& i : ((AST::Array*)expr)->elems )
        CalcConstexpr(i);
      break;

    default:
    {
      if( !expr->left->IsConstexpr() )
      {
        CalcConstexpr(expr->left);
        break;
      }

      if( !expr->right->IsConstexpr() )
      {
        CalcConstexpr(expr->right);
        break;
      }

      switch( expr->type )
      {

      }

      break;
    }
  }
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