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
      ast->expr = nullptr;

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

    return ast;
  }

  // instruction
  auto ast = new AST::Instruction;
  ast->name = tok.str;
  ast->token = &tok;
  next();

  if( !consume("\n") )
  {
    do
    {
      ast->args.emplace_back(Expr());
    } while( consume(",") );
    expect("\n");
  }

  return ast;
}

