#include "../main.h"

Object make_new_class_Obj(AST::Expr* ast)
{
  auto& className = ast->token->str;

  Object ret;
  ret.type = Object::ClassObj;

  for( auto&& i : Program::instance->classes ) {
    alart;
    if( i->token->str == className ) {
      ret.class_ptr = i;
      break;
    }
  }
  if( !ret.class_ptr )
    Program::Error(*ast->token, "undefined class");




  return ret;
}

AST::Class *ParserCore::Class()
{
  auto ast = new AST::Class;
  ast->token = &get_tok();

  alart;
  std::cout << ast->token->str << '\n';

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

  alart;
  classes.emplace_back(ast);

  return ast;
}

