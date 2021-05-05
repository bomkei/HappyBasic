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
    old_args.emplace_back(AST_Runner::Expr(ast->args[i]));
  }

  Program::instance->cur_func = ast;
  auto ret = AST_Runner::Stmt(ast->code);

  for( size_t i = 0; i < old_args.size(); i++ )
  {
    ast->args[i]->token->obj = old_args[i];
  }

  return ret;
}