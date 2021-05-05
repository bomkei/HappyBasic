#include "main.h"

Object AST_Runner::UserFunc(AST::Callfunc* fun)
{
  auto find = Program::instance->find_func(fun->token->str);

  if( find == -1 )
  {
    Program::Error(*fun->token, "undefined function");
  }


  return { };
}