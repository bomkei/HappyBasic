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

      break;

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
      
      break;

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
  }

  //
  // random
  if( name == "random" )
  {
    for( auto&& i : args )
      if( i.type != Object::Int )
        Program::Error(*ast->token, "only use integer in args of random()");

    auto begin = args.size() > 1 ? args[0].v_int : 0;
    auto end = args[args.size() > 1].v_int;

    if( begin > end )
      Program::Error(*ast->token, "begin > end");

    if( begin == end )
      ret.v_int = begin;
    else
      ret.v_int = Utils::Random(begin, end);
  }

  //
  // randomstr
  if( name == "randomstr" )
  {
    if( args.size() > 1 )
      Program::Error(*ast->token, "no matching args");

    auto len = args.size() ? args[0].v_int : 30;

    auto str = Utils::GetRandomStr<std::string>(len);

    ret.type = Object::Array;

    for( auto&& c : str )
    {
      Object ch;
      ch.type = Object::Char;
      ch.v_char = c;
      ret.list.emplace_back(ch);
    }
  }
  else
    return AST_Runner::UserFunc(ast);
  

  return ret;
}