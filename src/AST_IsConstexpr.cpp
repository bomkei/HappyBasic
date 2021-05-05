#include "main.h"

bool AST::Expr::IsConstexpr() const
{
  switch( type )
  {
  case Immidiate:
    return true;

  default:
    if( !left )
      return right ? right->IsConstexpr() : false;
    else if( !right )
      return left ? left->IsConstexpr() : false;

    return left->IsConstexpr() && right->IsConstexpr();
  }
  
  return false;
}