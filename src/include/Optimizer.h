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

  struct Alphabet
  {
    enum Type
    {
      Mul,
      Div
    };

    Type type;
    std::string name;

    Alphabet(std::string const& name)
      :type(Type::Mul), name(name)
    {

    }
  };

  bool IsHaveAlphabet(AST::Expr* expr, std::string const& name);
  void RemoveAlphabet(AST::Expr* expr, std::string const& name);

  std::vector<Term> GetTerms(AST::Expr* expr);
  std::vector<Alphabet> GetAlphabets(AST::Expr*);



}