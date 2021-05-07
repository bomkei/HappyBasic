#include "main.h"
#include <cassert>
#include <cmath>
#include <numeric>

/* internal types */
class SignedExpr
{
public:
  enum Sign
  {
    Positive = 1,
    Negative = -1,
  } sign;
  AST::Expr *expr;

  template <typename T>
  SignedExpr(T sign, AST::Expr *expr)
      : sign((Sign)sign), expr(expr)
  {
  }

  static SignedExpr FromExprRight(AST::Expr *expr)
  {
    auto right = expr->right;
    auto sign = (expr->type == AST::Expr::Add ? 1 : -1); // default sign (expr->type)

    if (right->type == AST::Expr::Immidiate)
    {
      sign *= right->token->obj.v_int > 0 ? 1 : -1; // if immidiate is negative, invert sign
    }

    return SignedExpr(sign, expr->right);
  }
};

class ExprType
{
public:
  enum Type
  {
    Expr,
    Term,
    Factor
  } type;

  bool isMatchedType(AST::Expr &src)
  {
    if (
        type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or
        type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div))
      return true;

    return false;
  }

  static bool isMatchedType(Type type, AST::Expr &src)
  {
    if (
        type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or
        type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div))
      return true;

    return false;
  }
};

class TypedExpr
{
public:
  enum Kind
  {
    Expr,
    Term
  } kind;

  enum Type
  {
    Normal = 1,
    Innormal = -1,
  } type;
  AST::Expr *expr;

  template <typename T>
  TypedExpr(T type, Kind kind, AST::Expr *expr)
      : type((Type)type), kind(kind), expr(expr)
  {
  }
  static Kind getKindFromExprType(ExprType srctype)
  {
    if (srctype.type == ExprType::Expr)
    {
      return Kind::Expr;
    }
    else if (srctype.type == ExprType::Term)
    {
      return Kind::Term;
    }
    else
    {
      return Kind::Expr; // it's default
    }
  }
  static TypedExpr FromExprRight(AST::Expr *expr)
  {
    Type type;
    Kind kind;
    AST::Expr::Type srctype = expr->type;
    if (srctype == AST::Expr::Mul or srctype == AST::Expr::Add)
    {
      type = Normal;
    }
    else if (srctype == AST::Expr::Div or srctype == AST::Expr::Sub)
    {
      type = Innormal;
    }

    if (srctype == AST::Expr::Mul or srctype == AST::Expr::Div)
    {
      kind = Term;
    }
    else
    {
      kind = Expr;
    }

    return TypedExpr(type, kind, expr->right);
  }
  int getSign()
  {
    return type == Normal ? 1 : -1;
  }
  AST::Expr::Type getType()
  {
    if (kind == Expr)
    {
      if (type == Normal)
      {
        return AST::Expr::Add;
      }
      else
      { // type == Innormal
        return AST::Expr::Sub;
      }
    }
    else
    { // kind == Term
      if (type == Normal)
      {
        return AST::Expr::Mul;
      }
      else
      { // type == Innormal
        return AST::Expr::Div;
      }
    }
  }
};

/* internal types - end */

void AST::Expr::Optimize()
{
  Expr ret;
  Expr *cur = &ret;
  // get expr type
  ExprType exprtype;
  if (type == Add or type == Sub)
    exprtype.type = ExprType::Expr;
  else if (type == Mul or type == Div)
    exprtype.type = ExprType::Term;
  else
    exprtype.type = ExprType::Factor;

  // get all parts
  std::vector<TypedExpr> parts;
  Expr *cur_left = this;
  while (exprtype.isMatchedType(*cur_left)) // loop while cur_left kind = Expr
  {
    parts.emplace_back(TypedExpr::FromExprRight(cur_left));
    cur_left = cur_left->left;
  }
  parts.emplace_back(TypedExpr(TypedExpr::Normal, TypedExpr::getKindFromExprType(exprtype), cur_left));

  if (exprtype.type == ExprType::Expr)
  {
    // calculate immidiate
    double immidiate = 0;
    for (auto it = parts.begin(); it != parts.end();)
    {
      if (it->expr->type == Immidiate)
      {
        immidiate += it->expr->token->obj.v_int * it->getSign();
        parts.erase(it);
      }
      else
        it++;
    }

    if (immidiate != 0)
    {
      cur->right = new Expr();
      cur->right->token = new Token();
      cur->right->token->obj.v_int = std::abs(immidiate);
      cur->type = immidiate > 0 ? Expr::Add : Expr::Sub;
      cur = cur->left = new Expr();
    }
  }
  else if (exprtype.type == ExprType::Term)
  {
    // remove immidiate!
    double imm_denom_dbl = 1;
    int imm_denom_int = 1;
    double imm_numer_dbl = 1;
    int imm_numer_int = 1;
    for (auto it = parts.begin(); it != parts.end();)
    {
      if (it->expr->type != Immidiate)
      { // skip not immidiate
        it++;
        continue;
      }
      parts.erase(it);

      auto obj = it->expr->token->obj;
      if (it->type == TypedExpr::Innormal)
      {
        if (obj.type == Object::Int)
          imm_numer_int *= obj.v_int;
        else
          imm_numer_dbl *= obj.as<double>();
      }
      else
      {
        if (obj.type == Object::Int)
          imm_denom_int *= obj.v_int;
        else
          imm_denom_dbl *= obj.as<double>();
      }
    }

    // imm
    int gcd = std::gcd(imm_denom_int, imm_numer_int);
    double imm_numer = imm_numer_dbl * (double)imm_numer_int / gcd;
    double imm_denom = imm_denom_dbl * (double)imm_denom_int / gcd;

    if (imm_numer != 1)
    {
      cur->right = new Expr();
      cur->right->token = new Token();
      cur->right->token->obj.type = Object::Float;
      cur->right->token->obj.v_float = imm_numer;
      cur->type = Expr::Mul;
      cur = cur->left = new Expr();
    }
    if (imm_denom != 1)
    {
      cur->right = new Expr();
      cur->right->token = new Token();
      cur->right->token->obj.type = Object::Float;
      cur->right->token->obj.v_float = imm_denom;
      cur->type = Expr::Div;
      cur = cur->left = new Expr();
    }
  }

  // reconstructing Expr (to ret)
  if (parts.size() == 1)
  {
    auto part = parts[0];
    // copy part.expr => ret
    ret = *part.expr;
  }
  else
  {
    int i = 0;
    for (auto &&part : parts)
    {
      if (parts.size() == ++i)
      {
        *cur = *part.expr;
      }
      else
      {
        cur->right = part.expr;
        cur->type = part.getType();
        cur = cur->left = new Expr();
      }
    }
  }

  std::cout << "check: " << *this << " | " << ret << std::endl;
  this->left = ret.left;
  this->type = ret.type;
  this->right = ret.right;
}