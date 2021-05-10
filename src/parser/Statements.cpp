#include "../main.h"

AST::Stmt* ParserCore::Statements()
{
  if( consume("if") )
    return If();

  if( consume("For") )
    return For();

  if( consume("While") )
    return While();

  if( consume("Do") )
    return DoWhile();

  if( consume("Func") )
    return Define();

  if( consume("Class") )
    return Class();

  if( consume("Var") ) {
    auto tk = csmtok;
    auto cc = Expr();
    if( cc->type != AST::Expr::Assign ) {
      Program::Error(*tk, "89t4308tg5342");
    }
    auto jjj = new AST::Stmt;
    jjj->type = AST::Stmt::Var;
    jjj->expr = cc;
    jjj->token = tk;
    expect("\n");
    return jjj;
  }

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

