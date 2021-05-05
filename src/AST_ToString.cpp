#include "main.h"

std::string AST::Expr::ToString() const
{
  switch( type )
  {
  case Immidiate:
    return token->obj.to_string();

  case Variable:
    return token->str;

  case Array:
  {
    std::string s;

    auto&& elems = ((AST::Array*)this)->elems;
    for( int i = 0; i < elems.size(); i++ )
    {
      s += elems[i]->ToString();
      if( i < elems.size() - 1 ) s += ", ";
    }

    return "[" + s + "]";
  }

  case Callfunc:
  {
    std::string s;
    auto&& args = ((AST::Callfunc*)this)->args;

    for( int i = 0; i < args.size(); i++ )
    {
      s += args[i]->ToString();
      if( i < args.size() - 1 ) s += ", ";
    }

    return token->str + "(" + s + ")";
  }

  case IndexRef:
  {
    return left->ToString() + "[" + right->ToString() + "]";
  }

  default:
  {
    auto left = this->left->ToString();
    auto right = this->right->ToString();

    return left+
      [] (Type t) {
      switch( t ) {
      case Add: return "+";
      case Sub: return "-";
      case Mul: return "*";
      case Div: return "/";
      case Shift: return "<<";
      case Bigger: return ">";
      case BiggerOrEqual: return ">=";
      case Equal: return "==";
      case NotEqual: return "!=";
      }
      return "?";
    }(this->type) + right;
  }

  }

  return "";
}