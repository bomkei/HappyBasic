#include "main.h"

void AST_Runner::Instruction(AST::Instruction* ast)
{
  auto const& name = ast->name;
  std::vector<Object> args;

  for( auto&& i : ast->args )
    args.emplace_back(AST_Runner::Expr(i));

  if( name == "input" )
  {
    
  }

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

  if( name == "insert" )
  {
    if( args.size() != 3 )
      Program::Error(*ast->token, "invalid arguments");

    if( !args[0].var_ptr || args[0].type != Object::Array )
      Program::Error(*(ast->args[0]->token), "only can use lvalue array on first argument");

    if( args[1].type != Object::Int )
      Program::Error(*(ast->args[1]->token), "index is must be a int");

    if( args[1].v_int < 0 || args[1].v_int >= args[0].var_ptr->list.size() )
      Program::Error(*(ast->args[1]->token), "out of range");

    args[0].var_ptr->list.insert(args[0].var_ptr->list.begin() + args[1].v_int, args[2]);
    return;
  }

  if( name == "clear" )
  {
    //for( auto&& var : Program::instance->variables )
    //  var = Object();
    
    auto size = Program::instance->variables.size();

    Program::instance->variables.clear();
    Program::instance->variables.resize(size);

    return;
  }

  if( name == "move" )
  {
    if( args.size() != 2 )
      Program::Error(*ast->token, "invalid arguments");

    if( !args[0].var_ptr )
      Program::Error(*ast->token, "cannot use rvalue at destination");

    if( args[1].type == Object::Array )
    {
      args[0].type = Object::Array;
      args[0].var_ptr->list = std::move(args[1].var_ptr->list);
    }

    return;
  }

  Program::Error(*ast->token, "undefined instruction");
}


