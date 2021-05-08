#include "../main.h"

AST::Block* ParserWrap::Parse()
{
  auto ast = new AST::Block;

  while( check() )
  {
    if( match("if") )
      ast->list.emplace_back(If());

    else if( match("for") )
      ast->list.emplace_back(For());

    else if( match("while") )
      ast->list.emplace_back(While());

    else if( match("do") )
      ast->list.emplace_back(DoWhile());

    else if( match("def") )
      ast->list.emplace_back(Define());

    else if( match("class") )
      ast->list.emplace_back(Class());

    else
      ast->list.emplace_back(Statements());

  }

  return ast;
}