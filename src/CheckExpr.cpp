#include "main.h"

using namespace AST;

void CheckStmt(Stmt* stmt) {
  if( !stmt ) {
    return;
  }

  switch( stmt->type ) {
    case Stmt::Block:
      for( auto&& i : ((Block*)stmt)->list )
        CheckStmt(i);
      break;

    case Stmt::If: {
      CheckExpr(((If*)stmt)->cond);
      CheckStmt(((If*)stmt)->code);
      CheckStmt(((If*)stmt)->elseCode);
      break;
    }

    case Stmt::For: {
      CheckExpr(((For*)stmt)->counter);
      CheckExpr(((For*)stmt)->begin);
      CheckExpr(((For*)stmt)->end);
      CheckStmt(((For*)stmt)->code);
      break;
    }

    case Stmt::While: {
      CheckExpr(((While*)stmt)->cond);
      CheckStmt(((While*)stmt)->code);
      break;
    }

    case Stmt::Function: {
      CheckStmt(((Function*)stmt)->code);
      break;
    }

    case Stmt::Break:
    case Stmt::Continue:
      break;
    
    case Stmt::Return:
      CheckExpr(stmt->expr);
      break;

    case Stmt::Struct:
      break;

    case Stmt::Var:
    default: {
      CheckExpr(stmt->expr);
      break;
    }
  }
}

void CheckExpr(Expr* expr) {

  if( !expr ) {
    return;
  }

  switch( expr->type ) {
    case Expr::Immidiate:
      break;
    
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