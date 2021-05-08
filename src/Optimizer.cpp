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


  bool IsHaveAlphabet(AST::Expr* expr, Alphabet const& alpha)
  {
    auto dont_r = false;
    auto res_r = false;

    if( !expr )
      return false;

    alart;
    std::cout << expr->token->str << '\n';

    //if( expr->type == AST::Expr::Variable )
    //  if( expr->token->str == alpha.name )
    //    return true;

    //return IsHaveAlphabet(expr->left, alpha) || IsHaveAlphabet(expr->right, alpha);

    if( expr->type == AST::Expr::Variable )
    {
      return expr->token->str == alpha.name;
    }
    else if( expr->right && expr->right->type == AST::Expr::Variable )
    {
      auto check = false;

      if( expr->type == AST::Expr::Mul )
      {
        if( alpha.type == Alphabet::Type::Mul )
          check = true;
      }
      else if( expr->type == AST::Expr::Div && alpha.type == Alphabet::Type::Div )
        check = true;

      if( check )
      {
        res_r = expr->right->token->str == alpha.name;
      }
    }

    auto left = IsHaveAlphabet(expr->left, alpha);

    if( !dont_r )
      left = left && IsHaveAlphabet(expr->right, alpha);

    return left;
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

  AST::Expr* ConstructAST_FromTerms(std::vector<Term> const& terms)
  {
    if( terms.empty() )
      return nullptr;

    auto ast = terms[0].term;

    for( size_t i = 1; i < terms.size(); i++ )
    {
      auto tk = new Token;
      tk->type = Token::Operator;
      tk->str = terms[i].sign == Term::Sign::Plus ? "+" : "-";

      ast =
        new AST::Expr(
          terms[i].sign == Term::Sign::Plus ? AST::Expr::Add : AST::Expr::Sub,
          ast, terms[i].term, tk
        );
    }

    return ast;
  }
}


void Debug(AST::Expr* expr)
{
  std::vector<Optimizer::Alphabet> Alphabets; // of terms

  auto terms = Optimizer::GetTerms(expr);
  

  std::cout << "\nterms(before):\n";
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

  std::cout << "\nreduce alphabets!\n";

  for( auto&& alpha : Alphabets )
  {
    std::vector<Optimizer::Term> alphaTerms;

    // search alpha in all terms
    for( auto it = terms.begin(); it < terms.end();)
    {
      // if have
      if( Optimizer::IsHaveAlphabet(it->term, alpha) )
      {
        alart;

        // remove
        Optimizer::RemoveAlphabet(it->term, alpha.name);

        alphaTerms.emplace_back(*it);
        terms.erase(it);
      }
      else
        it++;
    }

    auto ast = Optimizer::ConstructAST_FromTerms(alphaTerms);

    if( ast != nullptr )
    {
      //std::cout << "\nconstructed:\n";
      //std::cout << ast->ToString() << '\n';
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
