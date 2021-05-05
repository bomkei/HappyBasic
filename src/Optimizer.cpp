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

    case AST::Expr::Array:  // ???
    {
      for( auto&& i : ((AST::Array*)expr)->elems )
      {
        while( ReduceFactors(i) );
      }
      break;
    }

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

void Debug(AST::Expr* expr)
{

  // before
  std::cout << expr->ToString() << '\n';
  std::cout << "start optimize\n\n";

  //Optimizer::ReduceConstexpr(expr);

  auto terms = Optimizer::GetTermsFromExpr(expr);


  for( size_t i = 0; i < terms.size() - 1; )
  {
    if( terms[i].term->IsConstexpr() && terms[i + 1].term->IsConstexpr() )
    {
      auto ast = new AST::Expr;

      ast->type =
        (terms[i + 1].sign == Optimizer::Term::Sign::Minus) ? AST::Expr::Sub : AST::Expr::Add;

      ast->left = terms[i].term;
      ast->right = terms[i + 1].term;

      ast->left->token->obj = AST_Runner::Expr(ast);
      ast->left->type = AST::Expr::Immidiate;

      terms[i].term = ast->left;
      delete ast->right;

      terms.erase(terms.begin() + i + 1);
    }
    else if( !terms[i].term->IsConstexpr() )
    {
      while( Optimizer::ReduceFactors(terms[i].term) )
      {
        alart;
      }
      i++;
    }
    else
      i++;
  }

  alart;

  for( size_t i = 0; i < terms.size(); i++ )
  {
    if( terms[i].term->IsConstexpr() )
    {
      auto copy = terms[i];
      terms.erase(terms.begin() + i);
      terms.insert(terms.begin(), copy);
    }
  }

  alart;


  for( auto&& i : terms )
  {
  //  std::cout << i.term->ToString() << '\n';
  }

  alart;
  std::cout << expr->ToString() << '\n';

  alart;
  exit(100);
}
