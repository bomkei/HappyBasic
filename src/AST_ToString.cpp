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

bool AST::Expr::operator != (AST::Expr const& ast)const
{
  return (*this == ast) == false;
}

bool AST::Expr::operator == (AST::Expr const& _ast) const
{
  auto ast = (Expr*)&_ast;

  if( !ast )
    return false;

  if( ast->type != type )
    return false;

  std::cout << ToString() << ", " << ast->ToString() << '\n';

  switch( type )
  {
  case Immidiate:
    return token->obj.equal(ast->token->obj);

  case Variable:
    return varIndex == ast->varIndex;

  case Callfunc:
  {
    if( token->str != ast->token->str )
      return false;

    if( ((AST::Callfunc*)this)->args.size() != ((AST::Callfunc*)ast)->args.size() )
      return false;

    for( size_t i = 0; i < ((AST::Callfunc*)this)->args.size(); i++ )
      if( *((AST::Callfunc*)this)->args[i] != *((AST::Callfunc*)ast)->args[i] )
        return false;

    return true;
  }

  case Array:
  {
    if( ((AST::Array*)this)->elems.size() != ((AST::Array*)ast)->elems.size() )
      return false;

    for( size_t i = 0; i < ((AST::Array*)this)->elems.size(); i++ )
      if( *((AST::Array*)this)->elems[i] != *((AST::Array*)ast)->elems[i] )
        return false;

    return true;
  }

  default:
    return *left == *ast->left && *right == *ast->right;
  }

  return false;
}