#include "main.h"
#include <ostream>

std::ostream& operator<<(std::ostream& ss, const AST::Expr& expr)
{
  if( &expr == nullptr )
  {
    ss << "null";
    return ss;
  }

  if( expr.type >= AST::Expr::Add && AST::Expr::Div >= expr.type )
  {
    ss << *expr.left;
    ss << (expr.type == AST::Expr::Add    ? "+"
            : expr.type == AST::Expr::Sub ? "-"
            : expr.type == AST::Expr::Mul ? "*"
            : expr.type == AST::Expr::Div ? "/"
                                          : "?");
    ss << *expr.right;
  }
  else if( expr.type == AST::Expr::Immidiate )
  {
    auto token = expr.token;

    if( token == nullptr )
    {
      ss << "null";
      return ss;
    }

    switch( token->type )
    {
        //case Token::Operator:
        //  ss << "{" << token->str << "}";
        //  break;

      case Token::Ident:
        ss << token->str << "'";
        break;

      case Token::Char:
      case Token::Number:
      case Token::String:
        ss << token->obj.ToString();
        break;
    }
  }
  else if( expr.type == AST::Expr::Variable )
  {
    ss << "var[" << expr.varIndex << "](" << expr.token->str << ")";
  }
  else if( expr.type == AST::Expr::Callfunc )
  {
    auto callfunc = reinterpret_cast<AST::Callfunc*>((AST::Expr*)&expr);
    ss << callfunc->token->str << "(";

    for( size_t i = 0; i < callfunc->args.size(); i++ )
    {
      ss << *(callfunc->args[i]);
      if( i < callfunc->args.size() - 1 )
        ss << ", ";
    }

    ss << ")";
  }
  else if( expr.type == AST::Expr::Assign )
  {
    ss << *expr.left;
    ss << "=";
    ss << *expr.right;
  }
  return ss;
}
AST::Expr& AST::Expr::operator+=(AST::Expr& target)
{
  if( left == nullptr )
  {
    left = &target;
    return *this;
  }
  else if( right == nullptr )
  {
    type = Add;
    right = &target;
    return *this;
  }

  auto newObject = new AST::Expr;
  newObject->left = new AST::Expr();
  *newObject->left = *this;
  newObject->right = &target;
  newObject->type = Add;
  *this = *newObject;
  return *this;
}
AST::Expr& AST::Expr::operator*=(AST::Expr& target)
{
  if( left == nullptr )
  {
    left = &target;
    return *this;
  }
  else if( right == nullptr )
  {
    type = Mul;
    right = &target;
    return *this;
  }

  auto newObject = new AST::Expr;
  newObject->left = new AST::Expr();
  *newObject->left = *this;
  newObject->right = &target;
  newObject->type = Mul;
  *this = *newObject;
  return *this;
}