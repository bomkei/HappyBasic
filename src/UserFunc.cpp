#include "main.h"

Object AST_Runner::UserFunc(AST::Callfunc* fun)
{
  auto find = Program::instance->find_func(fun->token->str);

  if( find == -1 )
  {
    Program::Error(*fun->token, "undefined function");
  }

  auto ast = Program::instance->functions[find];
  std::vector<Object> old_args;

  for( size_t i = 0; i < old_args.size(); i++ )
  {
    old_args.emplace_back(ast->args[i]->token->obj);
  }

  for( size_t i = 0; i < fun->args.size(); i++ )
  {
    ast->args[i]->token->obj = AST_Runner::Expr(fun->args[i]);
  }

  Program::instance->cur_func = ast;
  
  Object ret;
  auto retp = AST_Runner::ReturnValue;

  AST_Runner::ReturnValue = &ret;
  
  //
  // Run
  AST_Runner::Stmt(ast->code);

  for( size_t i = 0; i < old_args.size(); i++ )
  {
    ast->args[i]->token->obj = old_args[i];
  }

  AST_Runner::ReturnValue = retp;
  return ret;
}