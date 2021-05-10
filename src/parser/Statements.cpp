#include "../main.h"

AST::Stmt* ParserCore::Statements()
{
  if( consume("if") )
    return If();

  if( match("For") )
    return For();

  if( match("While") )
    return While();

  if( match("Do") )
    return DoWhile();

  if( match("Func") )
    return Define();

  if( match("Class") )
    return Class();

  //
  // return
  if( consume("return") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Return;
    ast->token = csmtok;

    if( !consume("\n") )
    {
      ast->expr = Expr();
      expect("\n");
    }
    else
    {
      ast->expr = nullptr;
    }

    return ast;
  }

  //
  // break
  if( consume("break") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Break;
    ast->token = csmtok;
    expect("\n");
    return ast;
  }

  //
  // continue
  if( consume("continue") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Continue;
    ast->token = csmtok;
    expect("\n");
    return ast;
  }


  auto st = new AST::Stmt;

  st->expr = Expr();
  expect("\n");

  return st;
}

