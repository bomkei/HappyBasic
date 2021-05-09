#include "main.h"

namespace AST_Utils
{
  struct Alphabet
  {
    // true  = *
    // false = /
    bool IsNumerator;

    std::string Name;

    Alphabet(std::string const& name = "")
      :IsNumerator(true), Name(name)
    {
    }

    bool operator == (Alphabet const& a) const
    {
      return
        IsNumerator == a.IsNumerator &&
        Name == a.Name;
    }
  };

  struct Term
  {
    enum Sign
    {
      Plus,
      Minus
    };

    Sign sign;
    AST::Expr* ptr;

    Term(AST::Expr* p = nullptr)
      :sign(Sign::Plus), ptr(p)
    {

    }
  };

  std::vector<Term> GetTerms(AST::Expr* expr)
  {
    std::vector<Term> ret;

    if( !expr )
      return ret;

    // add / sub
    if( expr->type == AST::Expr::Add || expr->type == AST::Expr::Sub )
    {
      auto left = GetTerms(expr->left);
      auto right = GetTerms(expr->right);
      
      right[0].sign = expr->type == AST::Expr::Add ? Term::Sign::Plus : Term::Sign::Minus;

      ret.insert(ret.end(), left.begin(), left.end());
      ret.insert(ret.end(), right.begin(), right.end());

      return ret;
    }
    

    Term term;
    term.ptr = expr;
    ret.emplace_back(term);

    return ret;
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
      dont_r = true;
      
      Alphabet ab;
      ab.IsNumerator = expr->type == AST::Expr::Mul;
      ab.Name = expr->right->token->str;

      ret.emplace_back(ab);
    }

    for( auto&& alpha : GetAlphabets(expr->left) )
    {
      if( std::count(ret.begin(), ret.end(), alpha) == 0 )
      {
        ret.emplace_back(alpha);
      }
    }
    
    if( !dont_r )
    {
      for( auto&& alpha : GetAlphabets(expr->right) )
      {
        if( std::count(ret.begin(), ret.end(), alpha) == 0 )
        {
          ret.emplace_back(alpha);
        }
      }
    }

    return ret;
  }

  void RemoveAlphabet(AST::Expr* expr, Alphabet const& alpha)
  {
    if( !expr )
      return;

    if( expr->type == AST::Expr::Mul )
      if( !alpha.IsNumerator )
        return;

    if( !expr->right || !expr->right )
      return;

    if( expr->right->token->str == alpha.Name )
    {
      *expr = *expr->left;
      return;
    }

    RemoveAlphabet(expr->left, alpha);
  }

  
}

std::ostream& operator << (std::ostream& ost, AST_Utils::Alphabet const& a)
{
  ost << (a.IsNumerator ? "*" : "/") << " " << a.Name;
  return ost;
}

std::ostream& operator << (std::ostream& ost, AST_Utils::Term const& t)
{
  ost << (t.sign == AST_Utils::Term::Sign::Plus ? "+" : "-") << " " << t.ptr->ToString();
  return ost;
}

void Debug(AST::Expr* ast)
{
  using namespace AST_Utils;

  std::cout << "\nTerms:\n";
  auto terms = GetTerms(ast);
  for( auto&& t : terms )
  {
    std::cout << t << '\n';
  }

  


  exit(1);
}