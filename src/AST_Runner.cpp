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
    auto &var = Program::instance->variables[ast->varIndex];
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

Object AST_Runner::Run_Stmt(AST::Stmt* ast)
{
  if( !ast )
    return { };

  switch( ast->type )
  {
  case AST::Stmt::Block:
  {
    Object obj;

    for( auto&& i : ast->list )
      obj = Run_Stmt(i);

    return obj;
  }

  case AST::Stmt::Assign:
  {
    auto var = Run_Expr(((AST::Assign*)ast)->var);
    auto value = Run_Expr(((AST::Assign*)ast)->value);
    
    *(var.var_ptr) = value;
    
    return value;
  }

  case AST::Stmt::Default:
    return Run_Expr(ast->expr);


  }

  return { };
}



