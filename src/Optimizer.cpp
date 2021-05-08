#include "main.h"

namespace Optimizer
{
  std::vector<Term> GetTerms(AST::Expr* expr)
  {
    std::vector<Term> terms;

    if( expr->type != AST::Expr::Add && expr->type != AST::Expr::Sub )
    {
      terms.push_back(expr);
      return terms;
    }

    auto left = GetTerms(expr->left);
    auto right = GetTerms(expr->right);

    right[0].sign = expr->type == AST::Expr::Add ? Term::Sign::Plus : Term::Sign::Minus;

    for( auto&& t : left )
      terms.emplace_back(t);

    for( auto&& t : right )
      terms.emplace_back(t);

    return terms;
  }

  std::vector<std::string> GetAlphabets(AST::Expr* expr)
  {
    std::vector<std::string> ret;

    if( !expr )
      return ret;

    if( expr->type == AST::Expr::Variable )
    {
      ret.emplace_back(expr->token->str);
      return ret;
    }

    auto left = GetAlphabets(expr->left);
    auto right = GetAlphabets(expr->right);

    for( auto&& i : left ) ret.emplace_back(i);
    for( auto&& i : right ) ret.emplace_back(i);

    return ret;
  }
}


void Debug(AST::Expr* expr)
{
  std::vector<std::string> Alphabets; // of terms

  auto terms = Optimizer::GetTerms(expr);
  

  std::cout << "\nterms:\n";
  for( auto&& te : terms )
  {
    std::cout << (te.sign == Optimizer::Term::Sign::Plus ? "+" : "-") << " " << te.term->ToString() << '\n';
  }

  std::cout << "\nalphabets:\n";
  for( auto&& te : terms )
  {
    auto alpha = Optimizer::GetAlphabets(te.term);

    for( auto&& n : alpha )
    {
      if( std::count(Alphabets.begin(), Alphabets.end(), n) == 0 )
        Alphabets.emplace_back(n);
    }
  }

  for( auto&& name : Alphabets )
  {
    std::cout << name << '\n';
  }


  exit(10);
}
