#include "../main.h"

AST::Stmt* ParserCore::For()
{
  //
  // for
  if( consume("for") )
  {
    auto ast = new AST::For;
    ast->token = csmtok;

    ast->counter = Expr();

    expect("=");
    ast->begin = Expr();

    expect("to");
    ast->end = Expr();

    ast->code = new AST::Block;

    expect("\n");

    auto closed = false;

    while( check() )
    {
      if( consume("next") )
      {
        expect("\n");
        closed = true;
        break;
      }

      ast->code->list.emplace_back(Stmt());
    }

    if( !closed )
    {
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }

  return nullptr;
}
