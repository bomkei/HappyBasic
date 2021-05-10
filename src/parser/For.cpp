#include "../main.h"

AST::For* ParserCore::For()
{
  auto ast = new AST::For;
  ast->token = csmtok;

  ast->counter = IndexRef();

  expect("=");
  ast->begin = Expr();

  expect("To");
  ast->end = Expr();

  ast->code = new AST::Block;

  expect("\n");

  auto closed = false;

  while( check() )
  {
    if( consume("Next") )
    {
      expect("\n");
      closed = true;
      break;
    }

    ast->code->list.emplace_back(Statements());
  }

  if( !closed )
  {
    Program::Error(*ast->token, "not closed");
  }

  return ast;
}