#include "../main.h"

Object make_new_class_Obj(AST::Expr* ast)
{
  auto& className = ast->token->str;

  Object ret;
  ret.type = Object::ClassObj;

  for( auto&& i : Program::instance->classes ) {
    if( i->token->str == className ) {
      ret.class_ptr = i;
      break;
    }
  }
  if( !ret.class_ptr )
    Program::Error(*ast->token, "undefined class");

  for( auto&& i : ret.class_ptr->member_list ) {
    switch( i->type ) {

    }
  }

  return ret;
}

AST::Class *ParserCore::Class()
{
  auto ast = new AST::Class;
  ast->token = &get_tok();
  
  auto closed = false;
 
  next();
  expect("\n");

  auto flag = in_class;
  auto ptr = cur_class;

  in_class = true;
  cur_class = ast;

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

  classes.emplace_back(ast);

  in_class = flag;
  cur_class = ptr;

  return ast;
}

