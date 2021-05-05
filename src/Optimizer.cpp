#include "main.h"

Optimizer::Term::Term(AST::Expr* term)
  :term(term), sign(Sign::Plus)
{

}

// return: whether of reduced
bool Optimizer::ReduceFactors(AST::Expr* expr)
{
  // loop is continue if true !!

  switch( expr->type )
  {
    case AST::Expr::Immidiate:
    case AST::Expr::Variable:
      return false;

    case AST::Expr::Callfunc:
    {
      auto ret = false;

      for( auto&& arg : ((AST::Callfunc*)expr)->args )
      {
        if( ReduceFactors(arg) )
          ret = true;
      }

      return ret;
    }

    case AST::Expr::Array:
      break;

    default:
    {
      auto L = ReduceFactors(expr->left);
      auto R = ReduceFactors(expr->right);

      if( expr->left->IsConstexpr() && expr->right->IsConstexpr() )
      {
        expr->token->obj = AST_Runner::Expr(expr);
        expr->type = AST::Expr::Immidiate;

        delete expr->left;
        delete expr->right;
      }

      return L || R;
    }
  }

  return false;
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