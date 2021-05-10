#include "../main.h"

AST::If* ParserCore::If()
{
  AST::If::Pair pair;
  auto closed = false;

  auto ast = new AST::If;
  ast->token = csmtok;

  pair = std::make_pair(Expr(), new AST::Block);

  expect("Then");
  expect("\n");

  while( check() )
  {
    // End If
    if( consume("End") )
    {
      expect("If");
      expect("\n");
      ast->pairs.emplace_back(pair);
      closed = true;
      break;
    }

    // ElseIf
    else if( consume("ElseIf") )
    {
      ast->pairs.emplace_back(pair);

      pair = std::make_pair(Expr(), new AST::Block);

      expect("Then");
      expect("\n");
    }

    // Else
    else if( consume("Else") )
    {
      ast->pairs.emplace_back(pair);
      pair = std::make_pair(AST::Expr::FromInt(1), new AST::Block);

      expect("\n");

      while( check() )
      {
        if( consume("") )
        {
          expect("\n");
          closed = true;
          break;
        }

        std::get<1>(pair)->list.emplace_back(Statements());
      }

      ast->pairs.emplace_back(pair);
      break;
    }

    // stmt
    else
    {
      std::get<1>(pair)->list.emplace_back(Statements());
    }
  }

  if( !closed )
  {
    Program::Error(*ast->token, "not closed");
  }

  return ast;
}
