#include "main.h"

Object AST_Runner::UserFunc(AST::Callfunc* fun)
{
  auto find = Program::instance->find_func(fun->token->str);

  AST::Function* ast = nullptr;

  // not found
  if( find == -1 )
  {
    if( Program::instance->cur_class ) {
      for( auto&& i : Program::instance->cur_class->member_list ) {
        if( i->type == AST::Stmt::Function && ((AST::Function*)i)->name == fun->token->str ) {
          ast = ((AST::Function*)i);
          break;
        }
      }
    }

    if( !ast )
      Program::Error(*fun->token, "undefined function");
  }
  else ast = Program::instance->functions[find];


  //auto ast = Program::instance->functions[find];
  std::vector<Object> old_args;

  if( ast->args.size() != fun->args.size() )
    Program::Error(*fun->token, "no mathing arguments");

  // save args before run
  for( auto&& i : ast->args )
    old_args.emplace_back(i->token->obj);

  // set args
  for( size_t i = 0; i < fun->args.size(); i++ )
    ast->args[i]->token->obj = AST_Runner::Expr(fun->args[i]);

  Program::instance->cur_func = ast;
  
  Object ret;
  auto retp = AST_Runner::ReturnValue;
  auto flag = AST_Runner::FuncReturned;
  auto returnedFlag = false;

  AST_Runner::ReturnValue = &ret;
  AST_Runner::FuncReturned = &returnedFlag;
  
  CallCount++;

  if( Options::IsSafety && CallCount >= FUNC_CALL_DEPTH_MAX )
  {
    Program::Error(*fun->token, "prevented crash of stack overflow.");
  }

  //
  // Run
  AST_Runner::Stmt(ast->code);

  for( size_t i = 0; i < old_args.size(); i++ )
    ast->args[i]->token->obj = old_args[i];

  AST_Runner::ReturnValue = retp;
  AST_Runner::FuncReturned = flag;

  CallCount--;

  return ret;
}