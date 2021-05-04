#include "main.h"

Object AST_Runner::Function(AST::Callfunc* ast)
{
  auto& name = ast->token->str;
  std::vector<Object> args;
  Object ret;

  for( auto&& i : ast->args )
    args.emplace_back(Expr(i));

  //
  // range
  if( name == "range" )
  {
    ret.type = Object::Array;

    switch( args.size() )
    {
    case 1:
      if( args[0].type != Object::Int )
        Program::Error(*(ast->args[0]->token), "must be a integer");

      for( int i = 0; i < args[0].v_int; i++ )
      {
        Object obj;
        obj.v_int = i;
        ret.list.emplace_back(obj);
      }

      return ret;

    case 2:
      if( args[0].type != Object::Int )
        Program::Error(*(ast->args[0]->token), "must be a integer");

      if( args[1].type != Object::Int )
        Program::Error(*(ast->args[1]->token), "must be a integer");

      for( int i = args[0].v_int; i < args[1].v_int; i++ )
      {
        Object obj;
        obj.v_int = i;
        ret.list.emplace_back(obj);
      }

      return ret;

    default:
      Program::Error(*ast->token, "no matching args");
    }
  }

  //
  // length
  if( name == "length" )
  {
    if( args.size() != 1 )
      Program::Error(*ast->token, "no matching args");

    if( args[0].type != Object::Array )
      Program::Error(*(ast->args[0]->token), "this is not array");

    ret.v_int = args[0].list.size();
    return ret;
  }


  Program::Error(*ast->token, "undefined function");
}