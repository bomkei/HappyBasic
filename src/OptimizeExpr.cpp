#include "main.h"
#include <cassert>
#include <cmath>
#include <numeric>

/* internal types */

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

class InvertableExpr{
public:
  enum Type{
    NotInverted=1,
    Inverted=-1,
  } type;
  AST::Expr* expr;

  template <typename T>
  InvertableExpr(T type,AST::Expr* expr)
    : type((Type)type), expr(expr)
  {
  }

  static InvertableExpr FromExprRight(AST::Expr* expr){
    auto type=(expr->type==AST::Expr::Mul ? NotInverted : Inverted); // default sign (expr->type)
    return InvertableExpr(type,expr->right);
  }
};

/* internal types - end */

void AST::Expr::Optimize(){
  if(type==Add or type==Sub){
    // Expr
    // get all term
    std::vector<SignedExpr> terms;
    
    Expr *cur_left=this;
    while(cur_left->type == Add or cur_left->type==Sub)// loop while cur_left kind = Expr
    {
      assert(cur_left->type == Add or cur_left->type==Sub); // cur_left must be Add/Sub

      terms.emplace_back(SignedExpr::FromExprRight(cur_left));
      cur_left=cur_left->left;
    }
    terms.emplace_back(SignedExpr(SignedExpr::Positive,cur_left));

    // remove immidiate! (to immidiatePart)
    Expr *immidiatePart=new Expr();
    immidiatePart->token=new Token();
    for( auto it = terms.begin(); it != terms.end(); ) {
      if(it->expr->type==Immidiate){
        immidiatePart->token->obj.v_int+=it->expr->token->obj.v_int*it->sign;
        terms.erase(it);
      }else it++;
    }

    // Constructing Expr(for return!)
    Expr ret;
    ret.right=immidiatePart;
    ret.type = immidiatePart->token->obj.v_int > 0 ? Expr::Add : Expr::Sub;
    immidiatePart->token->obj.v_int=std::abs(immidiatePart->token->obj.v_int); // immidiatePart = |immidiatePart|

    Expr *cur=&ret;
    for (size_t i = 0; i < terms.size()-1; i++)
    {
      auto term=terms[i];
      cur->left=new Expr();
      cur=cur->left;
      cur->right=term.expr;
      cur->type=term.sign==1 ? Add : Sub;
    }
    cur->left=terms[terms.size()-1].expr;
    
    // apply changes
    this->right=ret.right;
    this->left=ret.left;
    this->type=ret.type;
  }else if(type==Mul or type==Div){
    // Term
    // get all factor
    std::vector<InvertableExpr> terms;
    
    Expr *cur_left=this;
    while(cur_left->type == Mul or cur_left->type==Div)// loop while cur_left kind = Expr
    {
      assert(cur_left->type == Mul or cur_left->type==Div); // cur_left must be Add/Sub

      terms.emplace_back(InvertableExpr::FromExprRight(cur_left));
      cur_left=cur_left->left;
    }
    terms.emplace_back(InvertableExpr(InvertableExpr::NotInverted,cur_left));

    // remove immidiate! (to immidiatePart)
    double imm_denom_dbl=1;int imm_denom_int=1;
    double imm_numer_dbl=1;int imm_numer_int=1;
    for( auto it = terms.begin(); it != terms.end(); ) {
      if(it->expr->type==Immidiate){
        if(it->type==InvertableExpr::NotInverted){
          if(it->expr->token->obj.type==Object::Int)
            imm_numer_int*=it->expr->token->obj.v_int;
          else
            imm_numer_dbl*=it->expr->token->obj.as<double>();
        }else{
          if(it->expr->token->obj.type==Object::Int)
            imm_denom_int*=it->expr->token->obj.v_int;
          else
            imm_denom_dbl*=it->expr->token->obj.as<double>();
        }
        terms.erase(it);
      }else it++;
    }
    int gcd=std::gcd(imm_denom_int, imm_numer_int);
    double imm_numer=imm_numer_dbl*(double)imm_numer_int/gcd;
    double imm_denom=imm_denom_dbl*(double)imm_denom_int/gcd;
  }else{
    // Factor
  }
}