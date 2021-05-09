#include "main.h"

namespace AST_Utils
{
  struct Alphabet
  {
    // true  = *
    // false = /
    bool IsNumerator;

    std::string Name;
    AST::Expr* ptr;

    Alphabet(std::string const& name = "", AST::Expr* ptr = nullptr)
      :IsNumerator(true), Name(name), ptr(ptr)
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
      ret.emplace_back(expr->token->str, expr);
      return ret;
    }
    else if( expr->right && expr->right->type == AST::Expr::Variable )
    {
      dont_r = true;
      
      Alphabet ab;
      ab.IsNumerator = expr->type == AST::Expr::Mul;
      ab.Name = expr->right->token->str;
      ab.ptr = expr->right;

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

  bool RemoveAlphabet(AST::Expr* expr, Alphabet const& alpha)
  {
    if( !expr )
      return false;

    if( expr->type == AST::Expr::Mul )
    {
      if( !alpha.IsNumerator )
        return false;
    }
    else if( expr->type == AST::Expr::Div )
      if( alpha.IsNumerator )
        goto L;

    if( !expr->right || !expr->right )
      return false;

    if( expr->right->token->str == alpha.Name )
    {
      *expr = *expr->left;
      return true;
    }
    else if( expr->left->token->str == alpha.Name )
    {
      *expr = *expr->right;
      return true;
    }

  L:
    return RemoveAlphabet(expr->left, alpha);
  }

  AST::Expr* ConstructAST(std::vector<Term> const& terms)
  {
    if( terms.empty() )
      return nullptr;

    auto ast = terms[0].ptr;

    for( size_t i = 1; i < terms.size(); i++ )
    {
      ast = new AST::Expr(terms[i].sign == Term::Plus ? AST::Expr::Add : AST::Expr::Sub, ast, terms[i].ptr, nullptr);
    }

    return ast;
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

  std::cout << "\nTerms(before):\n";
  auto terms = GetTerms(ast);
  for( auto&& t : terms )
  {
    std::cout << t << '\n';
  }

  std::cout << "\nAlphabets:\n";
  auto alphabets = GetAlphabets(ast);
  for( auto&& alpha : alphabets )
  {
    std::cout << alpha << '\n';
  }

  
  std::cout << "\nReduce alphabets!\n";

  std::vector<Term> after_terms;
  
  for( auto&& alpha : alphabets )
  {
    std::vector<Term> sub;
    
    for( auto it = terms.begin(); it != terms.end(); )
    {
      if( RemoveAlphabet(it->ptr, alpha) )
      {
        std::cout << "Reduced: " << alpha << '\n';
        std::cout << *it << '\n';

        sub.emplace_back(*it);
        terms.erase(it);

      }
      else
        it++;
    }

    auto ast_sub = ConstructAST(sub);
    ast_sub = new AST::Expr(AST::Expr::Mul, ast_sub, alpha.ptr, nullptr);

    std::cout << "|Reduce process is completed: " << alpha << '\n';
    std::cout << "| " << ast_sub->ToString() << '\n';

    alart;
    after_terms.emplace_back(Term(ast_sub));

    break;
  }

  alart;
  for( auto&& t : terms )
    after_terms.emplace_back(t);

  

  std::cout << "\nTerms(after):\n";
  for( auto&& t : after_terms )
  {
    std::cout << t << '\n';
  }

  alart;
  std::cout << after_terms.size() << '\n';

  *ast = *ConstructAST(after_terms);


  //exit(1);
}