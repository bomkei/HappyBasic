#include "../main.h"

Object make_new_class_Obj(std::string& name)
{

}

AST::Class *ParserCore::Class()
{
  auto ast = new AST::Class;
  ast->token = &get_tok();

  auto closed = false;
 
  next();
  expect("\n");

  while( check() ) {
    if( consume("End") ) {
      expect("Class");
      expect("\n");
      closed = true;
      break;
    }

    auto tk = &get_tok(); {
      if( tk->str != "Var" &&
        tk->str != "Func" &&
        tk->str != "Class" ) {
        Program::Error(*tk, "at9032j0t2j40");
      }
    }

    ast->member_list.emplace_back(Statements());
  }

  if( !closed )
    Program::Error(*ast->token, "not closed");

  return ast;
}

