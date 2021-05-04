#include "main.h"

Object AST_Runner::Run_Expr(AST::Expr* ast)
{
  if( !ast )
    return { };

  switch( ast->type )
  {
  case AST::Expr::Immidiate:
    return ast->token->obj;

  case AST::Expr::Variable:
  {
    auto& var = Program::instance->variables[ast->varIndex];
    var.var_ptr = &var;
    return var;
  }

  default:
  {
    auto left = Run_Expr(ast->left);
    auto right = Run_Expr(ast->right);

    switch( ast->type )
    {
    case AST::Expr::Add:
      left.v_int += right.v_int;
      break;

    case AST::Expr::Sub:
      left.v_int -= right.v_int;
      break;

    case AST::Expr::Mul:
      left.v_int *= right.v_int;
      break;

    case AST::Expr::Div:
      left.v_int /= right.v_int;
      break;

    }

    return left;
  }
  }

  return { };
}

void Instruction(AST::Instruction* ast)
{
  auto const& name = ast->name;
  std::vector<Object> args;

  for( auto&& i : ast->args )
    args.emplace_back(AST_Runner::Run_Expr(i));

  if( name == "print" )
  {
    for( auto&& i : args )
      std::cout << i.to_string();

    std::cout << '\n';
    return;
  }

  Program::Error(*ast->token, "undefined instruction");
}

Object AST_Runner::Run_Stmt(AST::Stmt* ast)
{
  if( !ast )
    return { };

  switch( ast->type )
  {
  case AST::Stmt::Block:
  {
    Object obj;

    for( auto&& i : ((AST::Block*)ast)->list )
      obj = Run_Stmt(i);

    return obj;
  }

  case AST::Stmt::Assign:
  {
    auto var = Run_Expr(((AST::Assign*)ast)->var);
    auto value = Run_Expr(((AST::Assign*)ast)->value);

    return *(var.var_ptr) = value;
  }

  case AST::Stmt::Instruction:
  {
    Instruction(reinterpret_cast<AST::Instruction*>(ast));
    break;
  }

  case AST::Stmt::If:
  {
    for( auto&& pair : ((AST::If*)ast)->pairs )
    {
      auto cond = Run_Expr(std::get<0>(pair));

      if( cond.eval() )
        return Run_Stmt(std::get<1>(pair));
    }

    break;
  }

  case AST::Stmt::For:
  {
    auto for_ast = reinterpret_cast<AST::For*>(ast);

    auto counter = Run_Expr(for_ast->counter);
    auto begin = Run_Expr(for_ast->begin);
    
    if( !counter.var_ptr )
      Program::Error(*(for_ast->counter->token), "not a lvalue");

    if( begin.type != Object::Int )
      Program::Error(*(for_ast->begin->token), "must be a integer");

    *(counter.var_ptr) = begin;

    while( 1 )
    {
      auto end = Run_Expr(for_ast->end);

      if( end.type != Object::Int )
        Program::Error(*(for_ast->end->token), "must be a integer");

      if( counter.var_ptr->v_int > end.v_int )
        break;

      Run_Stmt(for_ast->code);
      counter.var_ptr->v_int++;
    }

    return counter;
  }

  case AST::Stmt::While:
    break;

  default:
    return Run_Expr(ast->expr);


  }

  return { };
}



