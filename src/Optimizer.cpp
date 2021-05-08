#include "main.h"
//
//bool operator == (Optimizer::Alphabet const& a, Optimizer::Alphabet const& b)
//{
//  return a.name == b.name;
//}

namespace Optimizer
{
  bool Alphabet::operator == (Alphabet const& a)
  {
    return type == a.type && name == a.name;
  }


  bool IsHaveAlphabet(AST::Expr* expr, std::string const& name)
  {
    if( !expr )
      return false;

    if( expr->type == AST::Expr::Variable )
      if( expr->token->str == name )
        return true;

    return IsHaveAlphabet(expr->left, name) || IsHaveAlphabet(expr->right, name);
  }

  void RemoveAlphabet(AST::Expr* expr, std::string const& name)
  {
    if( !expr )
      return;
    
    if( expr->left && expr->right )
    {
      if( expr->left->token->str == name )
      {
        *expr = *expr->right;
        //return;
      }
      else if( expr->right->token->str == name )
      {
        *expr = *expr->left;
        //return;
      }
    }

    RemoveAlphabet(expr->left, name);
    RemoveAlphabet(expr->right, name);
  }

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

  std::vector<Alphabet> GetAlphabets(AST::Expr* expr)
  {
    std::vector<Alphabet> ret;
    auto dont_r = false;

    if( !expr )
      return ret;
    
    if( expr->type == AST::Expr::Variable )
    {
      ret.push_back(expr->token->str);
      return ret;
    }
    else if( expr->right && expr->right->type == AST::Expr::Variable )
    {
      Alphabet a;
      
      if( expr->type == AST::Expr::Mul )
        a.type = Alphabet::Type::Mul;
      else
        a.type = Alphabet::Type::Div;

      a.name = expr->right->token->str;
      ret.emplace_back(a);

      dont_r = true;
    }

    auto left = GetAlphabets(expr->left);
    
    for( auto&& t : left )
      ret.emplace_back(t);

    if( !dont_r )
    {
      for( auto&& t : GetAlphabets(expr->right) )
        ret.emplace_back(t);
    }

    return ret;
  }
}


void Debug(AST::Expr* expr)
{
  std::vector<Optimizer::Alphabet> Alphabets; // of terms

  auto terms = Optimizer::GetTerms(expr);
  

  std::cout << "\nterms:\n";
  for( auto&& te : terms )
  {
    std::cout << (te.sign == Optimizer::Term::Sign::Plus ? "+" : "-") << " " << te.term->ToString() << '\n';
  }

  std::cout << "\nalphabets:\n";
  for( auto&& te : terms )
  {
    // get alphabets
    auto alphas = Optimizer::GetAlphabets(te.term);

    // add to Alphabets if not exist
    for( auto&& n : alphas )
    {
      if( std::count(Alphabets.begin(), Alphabets.end(), n) == 0 )
        Alphabets.emplace_back(n);
    }
  }

  for( auto&& a : Alphabets )
  {
    std::cout << (a.type == Optimizer::Alphabet::Type::Mul ? "*" : "/") << " " << a.name << '\n';
  }

  std::cout << "\nreduce alphabets\n";

  for( auto&& alpha : Alphabets )
  {
    //std::vector<AST::Expr*> n_terms;

    for( auto&& term : terms )
    {
      if( Optimizer::IsHaveAlphabet(term.term, alpha.name) )
      {
        //n_terms.emplace_back(term);

        Optimizer::RemoveAlphabet(term.term, alpha.name);
      }
    }

    

    break;
  }

  std::cout << "\nterms(after):\n";
  for( auto&& te : terms )
  {
    std::cout << (te.sign == Optimizer::Term::Sign::Plus ? "+" : "-") << " " << te.term->ToString() << '\n';
  }



  exit(10);
}
