#include "main.h"
#include <cassert>
#include <cmath>
#include <numeric>

/* internal types */

class ExprType{
public:
  enum Type{
    Expr,
    Term,
    Factor
  }type;

  bool isMatchedType(AST::Expr &src){
    if(
      type==Expr and (src.type==AST::Expr::Add or src.type==AST::Expr::Sub) or
      type==Term and (src.type==AST::Expr::Mul or src.type==AST::Expr::Div)
    )return true;

    return false;
  }

  static bool isMatchedType(Type type,AST::Expr &src){
    if(
      type==Expr and (src.type==AST::Expr::Add or src.type==AST::Expr::Sub) or
      type==Term and (src.type==AST::Expr::Mul or src.type==AST::Expr::Div)
    )return true;

    return false;
  }
};

class TypedExpr{
public:
  enum Type{
    Normal=1,
    Innormal=-1,
  } type;
  AST::Expr* expr;

  template <typename T>
  TypedExpr(T type,AST::Expr* expr)
    : type((Type)type), expr(expr)
  {
  }

  static TypedExpr FromExprRight(AST::Expr* expr){
    Type type;
    AST::Expr::Type srctype=expr->type;
    if(srctype == AST::Expr::Mul or srctype == AST::Expr::Add){
      type=Normal;
    }else if(srctype == AST::Expr::Div or srctype == AST::Expr::Sub){
      type=Innormal;
    }
    
    return TypedExpr(type,expr->right);
  }
};

/* internal types - end */

void AST::Expr::Optimize(){
  ExprType exprtype;
  if(type==Add or type==Sub)
    exprtype.type=ExprType::Expr;
  else if(type==Mul or type==Div)
    exprtype.type=ExprType::Expr;
  else
    exprtype.type=ExprType::Factor;
  
  if(exprtype.isMatchedType(*this)){

  }
}