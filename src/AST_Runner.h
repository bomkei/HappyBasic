#pragma once

class AST_Runner
{
public:

  static Object Run_Expr(AST::Expr* ast)
  {
    if( !ast )
      return { };

    switch( ast->type )
    {
    case AST::Expr::Immidiate:
      return ast->token->obj;

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

  static Object Run_Stmt(AST::Stmt* ast)
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

    case AST::Stmt::Default:
      return Run_Expr(ast->expr);


    }

    return { };
  }



};