#include "../main.h"

AST::Stmt* ParserCore::Statements()
{
  if( match("if") )
    return If();

  if( match("for") )
    return For();

  if( match("while") )
    return While();

  if( match("do") )
    return DoWhile();

  if( match("def") )
    return Define();

  if( match("class") )
    return Class();

  //
  // return
  if( consume("return") )
  {
    auto ast = new AST::Return;
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
    expect("\n");
    return ast;
  }

  //
  // continue
  if( consume("continue") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Continue;
    expect("\n");
    return ast;
  }


  return Expr();
}

