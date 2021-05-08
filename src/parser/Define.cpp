#include "../main.h"

AST::Function* ParserCore::Define()
{
  //
  // def
  if( consume("def") )
  {
    auto tk = csmtok;

    if( in_function )
      Program::Error(*tk, "nested def");

    if( get_tok().type != Token::Ident )
      Program::Error(get_tok(), "expect identifier after 'def' keyword");

    auto& name = get_tok().str;
    next();

    std::vector<AST::Expr*> args;
    std::vector<AST::Stmt*> block;

    func_args = &args;

    expect("(");

    if( !consume(")") )
    {
      do
      {
        if( get_tok().type != Token::Ident )
          Program::Error(get_tok(), "syntax error");

        auto prm = new AST::Expr;
        prm->type = AST::Expr::Param;
        prm->token = &get_tok();
        next();

        args.emplace_back(prm);
      } while( consume(",") );

      expect(")");
    }

    expect("\n");

    auto closed = false;
    in_function = true;

    while( check() )
    {
      if( consume("end") )
      {
        closed = true;
        expect("\n");
        break;
      }

      block.emplace_back(Statements());
    }

    if( !closed )
      Program::Error(*tk, "not closed");

    in_function = false;

    auto ast = new AST::Function;
    ast->token = tk;
    ast->name = name;

    if( args.size() )
      ast->args = args;

    ast->code = new AST::Block;
    ast->code->list = std::move(block);

    functions.emplace_back(ast);
    return ast;
  }

  alart;

  return nullptr;
}
