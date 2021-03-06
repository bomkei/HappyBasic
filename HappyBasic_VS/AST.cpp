#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Global.h"


AST::Expr::Expr(Type type)
  : type(type)
  , left(nullptr)
  , right(nullptr)
  , token(nullptr)
{
}

AST::Expr::Expr(Type type, Expr* left, Expr* right, Token* tok)
  : type(type)
  , left(left)
  , right(right)
  , token(tok)
{
}

AST::Expr::Expr(float flt)
  : type(Immidiate)
  , left(nullptr)
  , right(nullptr)
  , token(nullptr)
{
  token = new Token();
  token->obj.type = Object::Float;
  token->obj.v_float = flt;
}

AST::Expr::Expr(int _int)
    : type(Immidiate)
    , left(nullptr)
    , right(nullptr)
    , token(nullptr)
{
  token = new Token();
  token->obj.type = Object::Int;
  token->obj.v_float = _int;
}

void AST::Expr::clear()
{
  type = Add;
  left = right = nullptr;
  token = nullptr;
}

void AST::Expr::fix()
{
  if( right == nullptr and left != nullptr )
  {
    *this = *left;
  }
  if( left == nullptr and right != nullptr )
  {
    *this = *right;
  }
}

bool AST::Expr::isPrimary()
{
  return type == Immidiate || type == Variable || type == Callfunc || type == IndexRef;
}

bool AST::Expr::isBinary()
{
  return type == Add || type == Sub || type == Mul || type == Div;
}

std::string AST::Expr::ToString() const
{
  switch( type )
  {
    case Immidiate:
      return token->obj.ToString();

    case Variable:
      return token->str;

    case Array:
    {
      std::string s;

      auto&& elems = ((AST::Array*)this)->elems;

      for( int i = 0; i < elems.size(); i++ )
      {
        s += elems[i]->ToString();
        if( i < elems.size() - 1 )
          s += ", ";
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
        if( i < args.size() - 1 )
          s += ", ";
      }

      return token->str + "(" + s + ")";
    }

    case IndexRef:
    {
      return left->ToString() + "[" + right->ToString() + "]";
    }

    case Assign:
      return left->ToString() + " = " + right->ToString();

    case MemberAccess:
    {
      return left->ToString() + "." + right->ToString();
    }

    default:
    {
      if( !left || !right )
      {
        Error(*token, "ERROR");
      }

      auto left = this->left->ToString();
      auto right = this->right->ToString();

      return "(" + left + ")" +
        [] (Type t) {
        switch( t )
        {
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
      }(this->type)
        + right;
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

    case Array:
      for( auto&& i : ((AST::Array*)this)->elems )
        if( !i->IsConstexpr() )
          return false;
      return true;

    case Variable:
      return false;

    default:
      if( !left )
      {
        if( right )
          return right->IsConstexpr();
      }
      else if( !right )
      {
        if( left )
          return left->IsConstexpr();
      }

      return left->IsConstexpr() && right->IsConstexpr();
  }

  return false;
}

bool AST::Expr::equal(AST::Expr const& ast) const
{
  if( ast.type != type )
    return false;

  switch( type )
  {
    case Immidiate:
      return token->obj.Equal(ast.token->obj);

    case Variable:
      return varIndex == ast.varIndex;

    case Callfunc:
    {
      auto&& args = ((AST::Callfunc*)this)->args;

      if( token->str != ast.token->str )
        return false;

      if( args.size() != ((AST::Callfunc*)&ast)->args.size() )
        return false;

      for( size_t i = 0; i < args.size(); i++ )
        if( !args[i]->equal(*((AST::Callfunc*)&ast)->args[i]) )
          return false;

      return true;
    }

    case Array:
    {
      auto& elems = ((AST::Array*)this)->elems;

      if( elems.size() != ((AST::Array*)&ast)->elems.size() )
        return false;

      for( size_t i = 0; i < elems.size(); i++ )
        if( !elems[i]->equal(*((AST::Array*)&ast)->elems[i]) )
          return false;

      return true;
    }

    default:
      return left->equal(*ast.left) && right->equal(*ast.right);
  }

  return false;
}

std::string AST::Stmt::ToString(int tab) const {
  auto&& space = std::string(tab * 2, ' ');

  switch( type ) {
    case Type::If:
    {
      auto ast = (AST::If*)this;

      return
        space + "if( " + ast->cond->ToString() + ")\n" +
        ast->code->ToString() + "\n" +
        (ast->elseCode ? "else \n" + ast->elseCode->ToString() : "");
    }

    case Type::For:
    {
      auto ast = (AST::For*)this;

      return
        space + "for( " + ast->iterator->ToString() + " in " + ast->list->ToString() + ")\n"
        + ast->code->ToString();
    }

    case Type::While:
    {
      auto ast = (AST::While*)this;

      return
        space + "while( " + ast->cond->ToString() + " )\n" + ast->code->ToString();
    }

    case Type::Var:
    {
      return space + "var " + expr->ToString();
    }

    case Type::Block:
    {
      std::string s;
      for( auto&& i : ((AST::Block*)this)->list ) s += i->ToString(tab + 1) + "\n";
      return "{\n" + s + "}";
    }

    case Type::Function:
    {
      auto ast = (AST::Function*)this;
      auto s = "def " + ast->name + "(";

      for( size_t i = 0; i < ast->args.size(); i++ ) {
        s += ast->args[i]->token->str;
        if( i < ast->args.size() - 1 ) s += ", ";
      }

      return s + ast->code->ToString();
    }

    case Type::Break:
    {
      return space + "break";
    }

    case Type::Continue:
    {
      return space + "continue";
    }

    case Type::Return:
    {
      return space + "return " + (expr ? expr->ToString() : "");
    }

    case Type::Struct:
    {
      auto ast = (AST::Struct*)this;
      auto s = "struct " + ast->name + " {\n";

      for( auto&& i : ast->member_list )
        s += i->ToString(tab + 1) + "\n";

      return s + "}";
    }

    default:
      return space + expr->ToString();
  }
}