#include "main.h"

using namespace AST;

void CheckExpr(Expr* expr) {

  if( !expr ) {
    return;
  }

  switch( expr->type ) {
    case Expr::Variable:
      Utils::VectorFindItem
  }

}