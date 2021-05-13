#include "main.h"

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
  return type == Immidiate or type == Variable or type == Callfunc or type == IndexRef;
}
bool AST::Expr::isBinary()
{
  return type == Add or type == Sub or type == Mul or type == Div;
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

    default:
    {
      if( !left || !right )
      {
        Program::Error(*token, "ERROR");
      }

      auto left = this->left->ToString();
      auto right = this->right->ToString();

      return "(" + left + ")" +
          [](Type t) {
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

std::string AST::Stmt::ToString(int tab) const
{
  auto space = std::string(tab, ' ');

  switch( type )
  {
    case Type::If:
    {
      std::string s = "if pairs:\n";

      for( auto&& p : ((AST::If*)this)->pairs )
      {
        s += " " + std::get<0>(p)->ToString() + ":\n" + " " + std::get<1>(p)->ToString(tab + 1);
      }

      return s + "\n";
    }

    case Type::For:
      return "for " + ((AST::For*)this)->counter->ToString() + " = " + ((AST::For*)this)->begin->ToString() + " to " + ((AST::For*)this)->end->ToString() + "\n" + ((AST::For*)this)->code->ToString(tab + 1) + "next\n";

    case Type::While:
      return "while " + ((AST::While*)this)->cond->ToString() + "\n"
          + ((AST::While*)this)->code->ToString(tab + 1) + "wend\n";

    case Type::Break:
      return "break\n";

    case Type::Continue:
      return "continue\n";

    case Type::Return:
      return "return" + expr->ToString() + "\n";

    case Type::Block: {
      std::string str;

      for( auto&& s : ((AST::Block*)this)->list ) {
        str += s->ToString(tab + 1);
      }

      return str + "\n";
    }

    case Type::Function:
    {
      std::string str;
      
      str += "def " + ((AST::Function*)this)->name + "("; {
        for( size_t i = 0; i < ((AST::Function*)this)->args.size(); i++ ) {
          str += ((AST::Function*)this)->args[i]->token->str;
          if( i < ((AST::Function*)this)->args.size() - 1 ) str += ", ";
        }
        str += ")\n";
      }

      str += ((AST::Function*)this)->code->ToString();

      return str + "end\n";
    }

    default:
      return space + expr->ToString() + '\n';
  }

  return "";
}