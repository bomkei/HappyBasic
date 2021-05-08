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

  alart;
  if( match("def") )
    return Define();

  alart;
  if( match("class") )
    return Class();

  alart;

  //
  // return
  if( consume("return") )
  {
    auto ast = new AST::Return;
    ast->token = csmtok;

    alart;

    if( !consume("\n") )
    {
      alart;
      ast->expr = Expr();

      alart;
      expect("\n");
    }
    else
    {

      alart;
      ast->expr = nullptr;
    }

    alart;
    return ast;
  }

  alart;

  //
  // break
  if( consume("break") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Break;
    expect("\n");
    return ast;
  }


  alart;

  //
  // continue
  if( consume("continue") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Continue;
    expect("\n");
    return ast;
  }

  alart;

  auto& tok = get_tok();

  if( tok.type != Token::Ident )
    Program::Error(tok, "expect instruction name or variable here");

  // assign
  if( index + 1 < tokens.size() && tokens[index + 1].str == "=" )
  {
    auto ast = new AST::Assign;
    ast->token = &tok;

    ast->var = Primary();
    next();

    ast->value = Expr();
    expect("\n");

    alart;
    return ast;
  }

  alart;

  // instruction
  auto ast = new AST::Instruction;
  ast->name = tok.str;
  ast->token = &tok;
  next();

  alart;

  if( !consume("\n") )
  {
    do
    {
      ast->args.emplace_back(Expr());
    } while( consume(",") );
    expect("\n");
  }

  alart;
  return ast;
}

