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
  enum Kind{
    Expr,
    Term
  } kind;
  
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
    Kind kind;
    AST::Expr::Type srctype=expr->type;
    if(srctype == AST::Expr::Mul or srctype == AST::Expr::Add){
      type=Normal;
    }else if(srctype == AST::Expr::Div or srctype == AST::Expr::Sub){
      type=Innormal;
    }
    
    if(srctype == AST::Expr::Mul or srctype == AST::Expr::Div){
      kind=Term;
    }else if(srctype == AST::Expr::Add or srctype == AST::Expr::Sub){
      kind=Expr;
    }
    
    return TypedExpr(type,expr->right);
  }
  int getSign(){
    return type==Normal?1:-1;
  }
};

/* internal types - end */

void AST::Expr::Optimize(){
  Expr ret;
  Expr *cur=&ret;
  // get expr type
  ExprType exprtype;
  if(type==Add or type==Sub)
    exprtype.type=ExprType::Expr;
  else if(type==Mul or type==Div)
    exprtype.type=ExprType::Expr;
  else
    exprtype.type=ExprType::Factor;
  
  // get all parts
  std::vector<TypedExpr> parts;
  Expr *cur_left=this;
  while(exprtype.isMatchedType(*cur_left))// loop while cur_left kind = Expr
  {
    parts.emplace_back(TypedExpr::FromExprRight(cur_left));
    cur_left=cur_left->left;
  }
  parts.emplace_back(TypedExpr(TypedExpr::Normal,cur_left));
  
  if(exprtype.type==ExprType::Expr){
    // calculate immidiate
    double immidiate=0;
    for( auto it = parts.begin(); it != parts.end(); ) {
      if(it->expr->type==Immidiate){
        immidiate+=it->expr->token->obj.v_int*it->getSign();
        parts.erase(it);
      }else it++;
    }
    
    if(immidiate != 0){
      cur->right=new Expr();
      cur->right->token=new Token();
      cur->right->token->obj.v_int=std::abs(immidiate);
      cur->type=immidiate>0?Expr::Add:Expr::Sub;
      cur=cur->left;
    }
  }

  // reconstructing Expr (to ret)
  int i=0;
  for (auto &&part : parts)
  {
    if(parts.size()==++i){
      // set to left
    }else{
      // cur->left=new Expr();
      // cur=cur->left;
      // cur->right=term.expr;
      // cur->type=term.type==1 ? Add : Sub;
    }
  }
  
  std::cout<<"check: "<<*this<<" | "<<ret<<std::endl;
  this->left=ret.left;
  this->type=ret.type;
  this->right=ret.right;
}