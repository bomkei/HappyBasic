#include "../main.h"

AST::If* ParserCore::If()
{
  //
  // if
  if( consume("if") )
  {
    AST::If::Pair pair;
    auto closed = false;

    auto ast = new AST::If;
    ast->token = csmtok;

    pair = std::make_pair(Expr(), new AST::Block);

    expect("then");
    expect("\n");

    while( check() )
    {
      if( consume("endif") )
      {
        expect("\n");
        ast->pairs.emplace_back(pair);
        closed = true;
        break;
      }
      else if( consume("elseif") )
      {
        ast->pairs.emplace_back(pair);

        pair = std::make_pair(Expr(), new AST::Block);

        expect("then");
        expect("\n");
      }
      else if( consume("else") )
      {
        ast->pairs.emplace_back(pair);
        pair = std::make_pair(AST::Expr::FromInt(1), new AST::Block);

        expect("\n");

        while( check() )
        {
          if( consume("endif") )
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

  return nullptr;
}
