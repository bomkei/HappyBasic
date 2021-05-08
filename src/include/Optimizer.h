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

    Alphabet(std::string const& name = "")
      :type(Type::Mul), name(name)
    {

    }

    bool operator == (Alphabet const&);
  };

  bool IsHaveAlphabet(AST::Expr*, Alphabet const&);
  void RemoveAlphabet(AST::Expr*, std::string const&);

  std::vector<Term> GetTerms(AST::Expr* expr);
  std::vector<Alphabet> GetAlphabets(AST::Expr*);

  AST::Expr* ConstructAST_FromTerms(std::vector<Term> const&);

}

//bool operator == (Optimizer::Alphabet, Optimizer::Alphabet);

