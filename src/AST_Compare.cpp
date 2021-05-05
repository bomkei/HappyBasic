#include "main.h"

bool operator == (AST::Expr* left, AST::Expr* right)
{
  if( !left || !right )
    return false;

  if( left->type != right->type )
    return false;

  switch( left->type )
  {
  case AST::Expr::Immidiate:
    return left->token->obj.equal(right->token->obj);

  case AST::Expr::Variable:
    return left->varIndex == right->varIndex;

  case AST::Expr::Callfunc:
    if( left->token->str != right->token->str )
      return false;

    for( auto&& arg : ((AST::Callfunc*)this)->args )
    {

    }
  }

  return false;
}