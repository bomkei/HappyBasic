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
    ss << "(";
    ss << *expr.left;
    ss << (expr.type == AST::Expr::Add    ? "+"
            : expr.type == AST::Expr::Sub ? "-"
            : expr.type == AST::Expr::Mul ? "*"
            : expr.type == AST::Expr::Div ? "/"
                                          : "?");
    ss << *expr.right;
    ss << ")";
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

      default:
        ss << token->obj.ToString();
        break;
    }
  }
  else if( expr.type == AST::Expr::Variable )
  {
    ss << expr.token->str;
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
AST::Expr& AST::Expr::_oprator(Type type, AST::Expr& target)
{
  if( Add <= this->type and this->type <= Div )
  {
    if( left == nullptr )
    {
      this->type = type;
      left = &target;
      return *this;
    }
    else if( right == nullptr )
    {
      this->type = type;
      right = &target;
      return *this;
    }
  }

  auto newObject = new AST::Expr;

  newObject->left = new AST::Expr();
  *newObject->left = *this;

  *this = *newObject;
  this->right = &target;
  this->type = type;
  return *this;
}
AST::Expr& AST::Expr::operator+=(AST::Expr& target)
{
  return _oprator(Add, target);
}
AST::Expr& AST::Expr::operator-=(AST::Expr& target)
{
  return _oprator(Sub, target);
}
AST::Expr& AST::Expr::operator*=(AST::Expr& target)
{
  return _oprator(Mul, target);
}
AST::Expr& AST::Expr::operator/=(AST::Expr& target)
{
  return _oprator(Div, target);
}