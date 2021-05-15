#include "main.h"

using namespace AST;

void CheckStmt(Stmt* stmt) {
  if( !stmt ) {
    return;
  }

  switch( stmt->type ) {
    case Block:
      for( auto&& i : ((Block*)stmt)->list )
        CheckStmt(i);
      break;

      
  }
}

void CheckExpr(Expr* expr) {

  if( !expr ) {
    return;
  }

  switch( expr->type ) {
    case Expr::Variable: {
      auto find = Program::GetInstance()->GetVariable(expr->token->str);

      if( !find ) {
        Program::GetInstance()->GetVariables().emplace_back(expr->token->obj);
      }

      break;
    }

    case Expr::Array: {
      for( auto&& i : ((AST::Array*)expr)->elems ) {
        CheckExpr(i);
      }
      break;
    }

    case Expr::Callfunc: {
      for( auto&& i : ((AST::Callfunc*)expr)->args ) {
        CheckExpr(i);
      }
      break;
    }

    default: {
      CheckExpr(expr->left);
      CheckExpr(expr->right);
    }
  }

}