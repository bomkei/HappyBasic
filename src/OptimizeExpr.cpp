#include "main.h"
#include <cassert>

/* internal types */
class SignedExpr{
public:
  enum Sign{
    Positive=1,
    Negative=-1,
  } sign;
  AST::Expr* expr;

  template <typename T>
  SignedExpr(T sign,AST::Expr* expr)
    : sign((Sign)sign), expr(expr)
  {
  }

  static SignedExpr FromExprRight(AST::Expr* expr){
    return SignedExpr(expr->right->type==AST::Expr::Add ? 1 : -1,expr->right);
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

    
  }else if(type==Mul or type==Div){
    // Term
  }else{
    // Factor
  }
}