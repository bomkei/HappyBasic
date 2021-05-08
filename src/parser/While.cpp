#include "../main.h"

AST::While* ParserCore::While()
{
  //
  // while
  if( consume("while") )
  {
    auto tk = csmtok;

    auto cond = Expr();
    expect("\n");

    std::vector<AST::Stmt*> block;
    auto closed = false;

    while( check() )
    {
      if( consume("wend") )
      {
        expect("\n");
        closed = true;
        break;
      }

      block.emplace_back(Statements());
    }

    if( !closed )
      Program::Error(*tk, "not closed");

    auto ast = new AST::While;
    ast->token = tk;
    ast->cond = cond;
    ast->code = new AST::Block;
    ast->code->list = std::move(block);

    return ast;
  }

  return nullptr;
}
