#include "main.h"

using namespace AST;

void CheckExpr(Expr* expr) {

  if( !expr ) {
    return;
  }

  switch( expr->type ) {
    case Expr::Variable: {
      auto find = -1;
      auto& vv = Program::GetInstance()->GetVariables();

      for( int i = 0; i < vv.size(); i++ ) {
        if( vv[i].name == expr->token->str ) {
          find = i;
          break;
        }
      }

      if( find == -1 ) {
        vv.emplace_back(expr->token->obj);
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

    case Expr::Assign: {

    }

  }

}