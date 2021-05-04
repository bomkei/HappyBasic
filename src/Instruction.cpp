#include "main.h"

void AST_Runner::Instruction(AST::Instruction* ast)
{
  auto const& name = ast->name;
  std::vector<Object> args;

  for( auto&& i : ast->args )
    args.emplace_back(AST_Runner::Expr(i));

  if( name == "print" )
  {
    for( auto&& i : args )
      std::cout << i.to_string();

    std::cout << '\n';
    return;
  }

  if( name == "push" )
  {
    if( args.size() < 2 )
      Program::Error(*ast->token, "invalid arguments");

    if( !args[0].var_ptr || args[0].type != Object::Array )
      Program::Error(*(ast->args[0]->token), "only can use lvalue array on first argument");

    for( int i = 1; i < args.size(); i++ )
    {
      args[0].var_ptr->list.emplace_back(args[i]);
    }

    return;
  }

  if( name == "pop" )
  {
    if( args.size() != 1 )
      Program::Error(*ast->token, "invalid arguments");

    if( !args[0].var_ptr || args[0].type != Object::Array )
      Program::Error(*(ast->args[0]->token), "only can use lvalue array on first argument");

    if( args[0].var_ptr->list.size() >= 1 )
      args[0].var_ptr->list.pop_back();

    return;
  }

  if( name == "clear" )
  {
    for( auto&& var : Program::instance->variables
      var = Object();

    return;
  }

  Program::Error(*ast->token, "undefined instruction");
}


