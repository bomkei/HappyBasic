#include "main.h"

Object AST_Runner::UserFunc(AST::Callfunc* fun) {
  AST::Function* ast = nullptr;

  if( Program::instance->cur_class ) {
    for( auto&& i : Program::instance->cur_class->member_list )
      if( i->type == AST::Stmt::Function && ((AST::Function*)i)->name == fun->token->str ) {
        ast = ((AST::Function*)i);
        break;
      }
  }
  else {
    auto find = Program::instance->find_func(fun->token->str);

    if( find != -1 ) {
      ast = Program::instance->functions[find];
    }
  }

  if( !ast )
    Program::Error(*fun->token, "undefined function");

  std::vector<Object> old_args;

  if( ast->args.size() != fun->args.size() ) {
    Program::Error(*fun->token,
      std::string(ast->args.size() > fun->args.size() ? "too few arguments" : "too many arguments") +
      ". declared is " + std::to_string(ast->args.size()) + " count" +
      " but this is written " + std::to_string(fun->args.size()) + " arguments");
  }

  // save args before run
  for( auto&& i : ast->args )
    old_args.emplace_back(i->token->obj);

  // set args
  for( size_t i = 0; i < fun->args.size(); i++ )
    ast->args[i]->token->obj = AST_Runner::Expr(fun->args[i]);

  auto cfunc = Program::instance->cur_func;
  Program::instance->cur_func = ast;
  
  Object ret;
  auto retp = AST_Runner::ReturnValue;
  auto flag = AST_Runner::FuncReturned;
  auto returnedFlag = false;

  AST_Runner::ReturnValue = &ret;
  AST_Runner::FuncReturned = &returnedFlag;
  
  CallCount++;

  if( Options::IsSafety && CallCount >= FUNC_CALL_DEPTH_MAX ) {
    Program::Error(*fun->token, "prevented crash of stack overflow.");
  }

  //
  // Run
  AST_Runner::Stmt(ast->code);

  for( size_t i = 0; i < old_args.size(); i++ )
    ast->args[i]->token->obj = old_args[i];

  AST_Runner::ReturnValue = retp;
  AST_Runner::FuncReturned = flag;
  Program::instance->cur_func = cfunc;

  CallCount--;

  return ret;
}